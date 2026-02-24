import json
import logging
import time
from datetime import datetime
from pathlib import Path

import pytest
import serial
from serial.tools.list_ports import comports

log = logging.getLogger("hub")

INSIGHT_HUB_VID = 0x303A
INSIGHT_HUB_PID = 0x1001
INSIGHT_HUB_PRODUCT = "InsightHUB Controller"

LOGS_DIR = Path(__file__).parent / "logs"

CONNECT_ATTEMPTS = 3
CONNECT_SETTLE_S = 0.5


def find_hub():
    for p in comports():
        if p.product == INSIGHT_HUB_PRODUCT or (
            p.vid == INSIGHT_HUB_VID and p.pid == INSIGHT_HUB_PID
        ):
            return p.device
    return None


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

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()


def pytest_addoption(parser):
    parser.addoption(
        "--port", default=None, help="Serial port for Insight Hub (auto-detect if omitted)"
    )


def pytest_configure(config):
    """Set up file logging for every test run."""
    LOGS_DIR.mkdir(exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    logfile = LOGS_DIR / f"run-{timestamp}.log"

    handler = logging.FileHandler(logfile)
    handler.setLevel(logging.DEBUG)
    handler.setFormatter(logging.Formatter(
        "%(asctime)s %(levelname)-7s %(name)s  %(message)s",
        datefmt="%H:%M:%S",
    ))
    logging.getLogger("hub").addHandler(handler)
    logging.getLogger("hub").setLevel(logging.DEBUG)

    # Store path so we can print it in the summary
    config._hub_logfile = logfile


def pytest_terminal_summary(terminalreporter, config):
    logfile = getattr(config, "_hub_logfile", None)
    if logfile:
        terminalreporter.write_sep("-", f"serial log: {logfile}")


def pytest_collection_modifyitems(session, config, items):
    """Connect to the hub before any tests run. Abort early on failure."""
    port = config.getoption("--port") or find_hub()
    if not port:
        pytest.exit("No Insight Hub found. Pass --port or connect a hub.", returncode=1)
    try:
        config._hub_instance = Hub(port)
    except (HubConnectionError, serial.SerialException) as e:
        pytest.exit(f"ERROR: {e}", returncode=1)


@pytest.fixture(scope="session")
def hub(request):
    h = request.config._hub_instance
    yield h
    h.close()
