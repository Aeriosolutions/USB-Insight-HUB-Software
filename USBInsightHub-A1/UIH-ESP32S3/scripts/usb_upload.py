"""PlatformIO custom upload via USB bootloader (no JTAG hardware needed).

Enters the ESP32-S3 ROM bootloader via 1200-baud touch, flashes firmware
with esptool, clears FORCE_DOWNLOAD_BOOT, and resets the chip so the
application boots.

Requires ``reboot_enabled=1`` in the hub's serial config.

Used as an extra_script by the ``esp32-s3-uih-usb`` PlatformIO environment.
"""

import os
import subprocess
import sys
import time
from datetime import datetime

Import("env")

# ESP32-S3 RTC register for FORCE_DOWNLOAD_BOOT flag
RTC_CNTL_OPTION1_REG = "0x6000812C"

# Log file for debugging upload issues
_LOG_DIR = os.path.join(env.subst("$PROJECT_DIR"), "logs")
os.makedirs(_LOG_DIR, exist_ok=True)
_LOG_PATH = os.path.join(
    _LOG_DIR,
    f"usb-upload-{datetime.now():%Y%m%d-%H%M%S}.log",
)
_LOG_FILE = open(_LOG_PATH, "w")


def _log(msg):
    """Print to console and log file."""
    print(msg)
    _LOG_FILE.write(msg + "\n")
    _LOG_FILE.flush()


def _run(cmd, **kwargs):
    """Run a subprocess, tee-ing output to the log file."""
    _LOG_FILE.write(f"$ {' '.join(cmd)}\n")
    _LOG_FILE.flush()
    r = subprocess.run(cmd, stdout=_LOG_FILE, stderr=subprocess.STDOUT, **kwargs)
    _LOG_FILE.write(f"exit code: {r.returncode}\n\n")
    _LOG_FILE.flush()
    return r


def _find_app_port():
    """Find the Insight Hub running the application firmware."""
    from serial.tools.list_ports import comports

    for p in comports():
        if p.vid == 0x303A and p.product == "InsightHUB Controller":
            return p.device
    return None


def _find_bootloader_port():
    """Find the ESP32-S3 ROM bootloader port."""
    from serial.tools.list_ports import comports

    for p in comports():
        if p.vid == 0x303A and p.product == "USB JTAG/serial debug unit":
            return p.device
    return None


def _enter_bootloader(port):
    """Enable reboot and do a 1200-baud touch to enter the ROM bootloader."""
    import json
    import termios

    import serial

    for attempt in range(3):
        try:
            s = serial.Serial(port, 115200, timeout=2)
            s.dtr = True
            time.sleep(0.5)
            s.reset_input_buffer()
            break
        except (serial.SerialException, termios.error, OSError):
            if attempt < 2:
                time.sleep(1.0)
            else:
                raise

    def _send(msg):
        payload = json.dumps(msg, separators=(",", ":")) + "\n"
        s.write(payload.encode())
        s.flush()
        return s.readline().decode("utf-8", errors="replace").strip()

    _log(f"Enabling reboot on {port}...")
    resp = _send({"action": "set", "params": {"reboot_enabled": 1}})
    _LOG_FILE.write(f"  reboot_enabled response: {resp}\n")
    if "ok" not in resp:
        s.close()
        sys.exit(f"Failed to enable reboot: {resp}")

    # Show "BOOT" on all displays so it's visible if the device gets stuck
    for ch in ("CH1", "CH2", "CH3"):
        _send({"action": "set", "params": {ch: {"Dev1_name": "BOOT", "numDev": 1}}})

    s.close()
    time.sleep(0.3)

    _log(f"1200-baud touch on {port}...")
    s = serial.Serial(port, 1200)
    time.sleep(0.5)
    s.close()
    time.sleep(3.0)

    # Wait for bootloader to enumerate
    deadline = time.monotonic() + 10.0
    while time.monotonic() < deadline:
        bl_port = _find_bootloader_port()
        if bl_port:
            return bl_port
        time.sleep(0.5)

    sys.exit("Bootloader did not enumerate after 1200-baud touch.")


def _clear_and_reset(esptool_path, python_path, port):
    """Clear FORCE_DOWNLOAD_BOOT and hard-reset via esptool."""
    # Clear the flag and reset in one esptool invocation using
    # --after hard_reset, which toggles RTS after the write_mem.
    _log("Clearing FORCE_DOWNLOAD_BOOT and resetting...")
    r = _run(
        [python_path, esptool_path,
         "--chip", "esp32s3",
         "--port", port,
         "--no-stub",
         "--after", "hard_reset",
         "write_mem", RTC_CNTL_OPTION1_REG, "0x0", "0x1"],
        timeout=30,
    )
    if r.returncode != 0:
        sys.exit(f"Failed to clear FORCE_DOWNLOAD_BOOT (rc={r.returncode})")
    time.sleep(1.0)


def usb_upload(source, target, env):
    """Custom upload action: 1200-baud touch → esptool flash → reset."""
    esptool_path = os.path.join(
        env.PioPlatform().get_package_dir("tool-esptoolpy") or "", "esptool.py"
    )
    python_path = env.subst("$PYTHONEXE")

    _log(f"Upload log: {_LOG_PATH}")

    # Determine device state
    bl_port = _find_bootloader_port()
    if bl_port:
        _log(f"Device already in bootloader on {bl_port}")
    else:
        app_port = _find_app_port()
        if not app_port:
            app_port = env.subst("$UPLOAD_PORT")
            if not app_port:
                sys.exit(
                    "Hub not found. Connect the hub and ensure it is running."
                )
        bl_port = _enter_bootloader(app_port)

    _log(f"Flashing via bootloader on {bl_port}...")

    # Build the esptool write_flash command
    flash_args = [
        python_path, esptool_path,
        "--chip", "esp32s3",
        "--port", bl_port,
        "--baud", env.subst("$UPLOAD_SPEED") or "921600",
        "--no-stub",
        "--after", "no_reset",
        "write_flash", "-z",
        "--flash_mode", env.subst("${__get_board_flash_mode(__env__)}"),
        "--flash_freq", env.subst("${__get_board_f_image(__env__)}"),
        "--flash_size", env.BoardConfig().get("upload.flash_size", "detect"),
    ]

    # Collect firmware images and offsets
    build_dir = env.subst("$BUILD_DIR")
    framework_dir = env.PioPlatform().get_package_dir("framework-arduinoespressif32")

    images = [
        ("0x0",     os.path.join(build_dir, "bootloader.bin")),
        ("0x8000",  os.path.join(build_dir, "partitions.bin")),
        ("0xe000",  os.path.join(framework_dir, "tools", "partitions", "boot_app0.bin")),
        ("0x10000", str(source[0])),
    ]

    for offset, path in images:
        if not os.path.isfile(path):
            sys.exit(f"Missing image: {path}")
        flash_args.extend([offset, path])

    r = _run(flash_args, timeout=120)
    if r.returncode != 0:
        _log(f"esptool failed — see {_LOG_PATH}")
        sys.exit(f"esptool flash failed (rc={r.returncode})")

    # Clear FORCE_DOWNLOAD_BOOT and reset into application
    _clear_and_reset(esptool_path, python_path, bl_port)

    # Wait for application to come up
    _log("Waiting for hub to boot...")
    deadline = time.monotonic() + 30.0
    while time.monotonic() < deadline:
        app_port = _find_app_port()
        if app_port:
            _log(f"Hub running on {app_port}")
            _LOG_FILE.close()
            return
        time.sleep(0.5)

    _log(f"Warning: hub did not re-enumerate within 30s — see {_LOG_PATH}")
    _LOG_FILE.close()


env.Replace(UPLOADCMD=usb_upload)
env.Replace(
    UPLOAD_PROTOCOL="custom",
)
