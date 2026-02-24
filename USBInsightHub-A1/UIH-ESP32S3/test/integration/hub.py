"""Hub connection class and device discovery utilities.

This module is independent of pytest and can be used by any test or script
that needs to communicate with the Insight Hub over serial.
"""

import json
import logging
import shutil
import subprocess
import time
from pathlib import Path

import serial
from serial.tools.list_ports import comports

log = logging.getLogger("hub")

INSIGHT_HUB_VID = 0x303A
INSIGHT_HUB_PID = 0x1001
INSIGHT_HUB_PRODUCT = "InsightHUB Controller"
BOOTLOADER_PRODUCT = "USB JTAG/serial debug unit"

CONNECT_ATTEMPTS = 3
CONNECT_SETTLE_S = 0.5

# NVS partition layout (from partition_uih_8MB.csv)
NVS_OFFSET = 0x9000
NVS_SIZE = 0x5000


def find_hub():
    """Find the Insight Hub's serial port by USB VID/PID or product string.

    Excludes devices in ROM bootloader mode (same VID/PID but different
    product string).
    """
    for p in comports():
        if p.product == BOOTLOADER_PRODUCT:
            continue
        if p.product == INSIGHT_HUB_PRODUCT or (
            p.vid == INSIGHT_HUB_VID and p.pid == INSIGHT_HUB_PID
        ):
            return p.device
    return None


def find_bootloader():
    """Find an ESP32-S3 ROM bootloader serial port.

    WARNING: If multiple ESP32-S3 devices are connected (e.g. hub + collar),
    this may return the wrong one. Use --port to specify explicitly when
    multiple devices are present.
    """
    for p in comports():
        if p.product == BOOTLOADER_PRODUCT and p.vid == INSIGHT_HUB_VID:
            return p.device
    return None


def find_esptool():
    """Find esptool.py via PATH, then PlatformIO discovery."""
    for name in ("esptool.py", "esptool"):
        found = shutil.which(name)
        if found:
            return found
    pio = shutil.which("pio") or shutil.which("platformio")
    if pio:
        try:
            r = subprocess.run(
                [pio, "system", "info", "--json-output"],
                capture_output=True, text=True, timeout=10,
            )
            info = json.loads(r.stdout)
            core_dir_entry = info.get("core_dir", {})
            core_dir_str = (core_dir_entry.get("value", "")
                           if isinstance(core_dir_entry, dict)
                           else str(core_dir_entry))
            core_dir = Path(core_dir_str)
            for candidate in [
                core_dir / "packages" / "tool-esptoolpy" / "esptool.py",
                core_dir / "penv" / "bin" / "esptool.py",
                core_dir / "penv" / "Scripts" / "esptool.py",
            ]:
                if candidate.exists():
                    return str(candidate)
        except Exception:
            pass
    return None


def _find_pio_python():
    """Return PlatformIO's bundled Python interpreter path."""
    pio_python = Path.home() / ".platformio" / "penv" / "bin" / "python3"
    return str(pio_python) if pio_python.exists() else "python3"


class HubConnectionError(Exception):
    pass


class Hub:
    """Thin wrapper around the Insight Hub's JSON serial API."""

    def __init__(self, port, timeout=2.0):
        self.port = port
        self.timeout = timeout
        self.ser = None
        self._connect()
        self._verify_responsive()

    def _connect(self):
        log.info("Opening %s (timeout=%.1fs)", self.port, self.timeout)
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.ser = serial.Serial(self.port, 115200, timeout=self.timeout)
        self.ser.dtr = True
        time.sleep(CONNECT_SETTLE_S)
        self.ser.reset_input_buffer()
        log.info("Opened %s", self.port)

    def _dtr_reset(self):
        """Toggle DTR to trigger a CDC reconnect on the ESP32-S3."""
        log.info("DTR reset on %s", self.port)
        self.ser.dtr = False
        time.sleep(0.1)
        self.ser.dtr = True
        time.sleep(CONNECT_SETTLE_S)
        self.ser.reset_input_buffer()

    def _verify_responsive(self):
        """Probe the hub with a simple get; retry with DTR reset if needed."""
        for attempt in range(1, CONNECT_ATTEMPTS + 1):
            log.info("Connection check attempt %d/%d", attempt, CONNECT_ATTEMPTS)
            resp = self.send({"action": "get", "params": ["hubMode"]})
            if resp and resp.get("status") == "ok":
                log.info("Hub responsive on attempt %d", attempt)
                return
            if attempt < CONNECT_ATTEMPTS:
                log.warning("Hub not responding, retrying with DTR reset...")
                self._dtr_reset()

        raise HubConnectionError(
            f"Hub on {self.port} not responding after {CONNECT_ATTEMPTS} attempts. "
            f"Try power-cycling the hub (unplug/replug USB)."
        )

    def send(self, msg):
        payload = json.dumps(msg, separators=(",", ":")) + "\n"
        log.debug("TX: %s", payload.rstrip())
        self.ser.write(payload.encode())
        self.ser.flush()
        line = self.ser.readline().decode("utf-8", errors="replace").strip()
        if line:
            log.debug("RX: %s", line)
            try:
                return json.loads(line)
            except json.JSONDecodeError:
                log.warning("RX not valid JSON: %r", line)
                return None
        log.debug("RX: (timeout, no response)")
        return None

    def get(self, *params):
        resp = self.send({"action": "get", "params": list(params)})
        if resp and resp.get("status") == "ok":
            return resp.get("data", {})
        log.warning("get(%s) failed: %r", params, resp)
        return None

    def set(self, params):
        resp = self.send({"action": "set", "params": params})
        ok = resp and resp.get("status") == "ok"
        if not ok:
            log.warning("set(%s) failed: %r", params, resp)
        return ok

    def reconnect(self, timeout=30.0, poll_interval=0.5):
        """Wait for hub to reboot, then reconnect.

        Phase 1: wait up to 5s for the USB port to disappear (the hub
        may or may not disconnect depending on reset type — ESP32-S3
        USB-JTAG/Serial stays powered through software resets).

        Phase 2: poll until the application port appears and the hub
        responds to a command.
        """
        log.info("Reconnecting (timeout=%.1fs)", timeout)
        self.close()
        deadline = time.monotonic() + timeout

        # Phase 1: brief wait for port to disappear (may not happen)
        phase1_deadline = min(time.monotonic() + 5.0, deadline)
        while time.monotonic() < phase1_deadline:
            if not find_hub():
                log.info("Hub disconnected from USB")
                break
            time.sleep(poll_interval)

        # Phase 2: wait for the port to (re)appear
        port = None
        while time.monotonic() < deadline:
            port = find_hub()
            if port:
                break
            time.sleep(poll_interval)
        if not port:
            raise HubConnectionError(
                f"Hub did not re-enumerate within {timeout}s"
            )
        log.info("Hub re-enumerated on %s", port)
        self.port = port
        self._connect()
        self._verify_responsive()

    def touch_1200(self):
        """Open port at 1200 baud to trigger bootloader entry.

        Requires reboot_enabled=1 to be set first.  Only changes baud rate
        to 1200 to trigger the _onLineCoding path.  Does NOT toggle DTR/RTS
        to avoid triggering the separate line-state bootloader sequence.
        """
        log.info("1200-baud touch on %s", self.port)
        self.close()
        s = serial.Serial(self.port, 1200)
        time.sleep(0.5)
        s.close()
        time.sleep(2.0)

    def boot_from_bootloader(self):
        """Exit ROM bootloader via esptool hard reset.

        Clears FORCE_DOWNLOAD_BOOT via write_mem and triggers a hard reset
        so the chip boots the application.
        """
        bl_port = find_bootloader()
        if not bl_port:
            app_port = find_hub()
            if app_port:
                log.info("Hub already running (not in bootloader) on %s", app_port)
                self.port = app_port
                self._connect()
                self._verify_responsive()
                return
            raise HubConnectionError(
                "Hub not found — neither bootloader nor application port detected."
            )
        log.info("Recovering from bootloader on %s", bl_port)
        self.close()

        esptool_path = find_esptool()
        if not esptool_path:
            raise RuntimeError("esptool not found — cannot recover from bootloader.")

        pio_python = _find_pio_python()

        log.info("Clearing FORCE_DOWNLOAD_BOOT and resetting on %s", bl_port)
        cmd = [pio_python, esptool_path,
               "--port", bl_port, "--chip", "esp32s3",
               "--no-stub", "--after", "hard_reset",
               "write_mem", "0x6000812C", "0x0", "0x1"]
        log.debug("Running: %s", " ".join(cmd))
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        log.debug("esptool stdout: %s", r.stdout.strip())
        if r.returncode != 0:
            log.warning("esptool stderr: %s", r.stderr.strip())
            raise RuntimeError(
                f"esptool write_mem failed (rc={r.returncode}): {r.stderr.strip()}"
            )
        time.sleep(1.0)

        deadline = time.monotonic() + 30.0
        port = None
        while time.monotonic() < deadline:
            port = find_hub()
            if port:
                break
            time.sleep(0.5)
        if not port:
            raise HubConnectionError(
                "Hub did not re-enumerate after hard reset. "
                "Try unplugging and replugging the hub."
            )
        log.info("Hub re-enumerated on %s after hard reset", port)
        self.port = port
        self._connect()
        self._verify_responsive()

    def dump_nvs(self, output_path):
        """Dump NVS partition to a binary file via esptool.

        Enters bootloader mode (1200-baud touch), reads flash, then
        exits bootloader back to the application.

        Requires reboot_enabled=1 to be set first.
        """
        output_path = Path(output_path)
        output_path.parent.mkdir(parents=True, exist_ok=True)

        esptool_path = find_esptool()
        if not esptool_path:
            raise RuntimeError("esptool not found — cannot dump NVS.")
        pio_python = _find_pio_python()

        # Enter bootloader
        log.info("Entering bootloader for NVS dump")
        self.touch_1200()

        bl_port = None
        deadline = time.monotonic() + 10.0
        while time.monotonic() < deadline:
            bl_port = find_bootloader()
            if bl_port:
                break
            time.sleep(0.5)
        if not bl_port:
            raise HubConnectionError("Bootloader port not found after 1200-baud touch")

        # Read NVS partition
        log.info("Reading NVS partition (0x%X, %d bytes) from %s",
                 NVS_OFFSET, NVS_SIZE, bl_port)
        cmd = [pio_python, esptool_path,
               "--port", bl_port, "--chip", "esp32s3",
               "--no-stub", "--after", "no_reset",
               "read_flash", hex(NVS_OFFSET), hex(NVS_SIZE), str(output_path)]
        log.debug("Running: %s", " ".join(cmd))
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        log.debug("esptool stdout: %s", r.stdout.strip())
        if r.returncode != 0:
            log.warning("esptool stderr: %s", r.stderr.strip())
            raise RuntimeError(
                f"esptool read_flash failed (rc={r.returncode}): {r.stderr.strip()}"
            )
        log.info("NVS dump saved to %s (%d bytes)",
                 output_path, output_path.stat().st_size)

        # Exit bootloader, return to app
        self.boot_from_bootloader()

    def restore_nvs(self, input_path):
        """Restore NVS partition from a binary file via esptool.

        Enters bootloader mode (1200-baud touch), writes flash, then
        exits bootloader back to the application.

        Requires reboot_enabled=1 to be set first.
        """
        input_path = Path(input_path)
        if not input_path.exists():
            raise FileNotFoundError(f"NVS snapshot not found: {input_path}")

        esptool_path = find_esptool()
        if not esptool_path:
            raise RuntimeError("esptool not found — cannot restore NVS.")
        pio_python = _find_pio_python()

        # Enter bootloader
        log.info("Entering bootloader for NVS restore")
        self.touch_1200()

        bl_port = None
        deadline = time.monotonic() + 10.0
        while time.monotonic() < deadline:
            bl_port = find_bootloader()
            if bl_port:
                break
            time.sleep(0.5)
        if not bl_port:
            raise HubConnectionError("Bootloader port not found after 1200-baud touch")

        # Write NVS partition
        log.info("Writing NVS partition (0x%X) from %s to %s",
                 NVS_OFFSET, input_path, bl_port)
        cmd = [pio_python, esptool_path,
               "--port", bl_port, "--chip", "esp32s3",
               "--no-stub", "--after", "hard_reset",
               "write_flash", hex(NVS_OFFSET), str(input_path)]
        log.debug("Running: %s", " ".join(cmd))
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        log.debug("esptool stdout: %s", r.stdout.strip())
        if r.returncode != 0:
            log.warning("esptool stderr: %s", r.stderr.strip())
            raise RuntimeError(
                f"esptool write_flash failed (rc={r.returncode}): {r.stderr.strip()}"
            )
        log.info("NVS restored from %s", input_path)

        # Wait for app to boot after hard reset
        time.sleep(1.0)
        deadline = time.monotonic() + 30.0
        port = None
        while time.monotonic() < deadline:
            port = find_hub()
            if port:
                break
            time.sleep(0.5)
        if not port:
            raise HubConnectionError("Hub did not re-enumerate after NVS restore")
        self.port = port
        self._connect()
        self._verify_responsive()


    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
