import logging
from datetime import datetime
from pathlib import Path

import pytest
import serial

from hub import Hub, HubConnectionError, find_hub

LOGS_DIR = Path(__file__).parent / "logs"


PROJECT_DIR = Path(__file__).parent.parent.parent  # UIH-ESP32S3/


def pytest_addoption(parser):
    parser.addoption(
        "--port", default=None, help="Serial port for Insight Hub (auto-detect if omitted)"
    )
    parser.addoption(
        "--quick", action="store_true", default=False,
        help="Skip slow tests (restart, bootloader recovery)",
    )
    parser.addoption(
        "--host", default=None,
        help="Hub hostname/IP for OTA upgrade tests (e.g. insighthub.local)",
    )
    parser.addoption(
        "--old-firmware",
        default=str(PROJECT_DIR / "build" / "firmware" / "USBInsightHub-A0_esp32-s3-uih_1-0-0.bin"),
        help="Path to v1.0.0 firmware binary (for upgrade tests)",
    )
    parser.addoption(
        "--new-firmware",
        default=str(PROJECT_DIR / ".pio" / "build" / "esp32-s3-uih" / "firmware.bin"),
        help="Path to current firmware binary (for upgrade tests)",
    )


def pytest_configure(config):
    """Set up file logging and custom markers."""
    config.addinivalue_line("markers", "slow: marks tests that reboot the hub or take >10s")

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

    if config.getoption("--quick"):
        skip_slow = pytest.mark.skip(reason="skipped by --quick")
        for item in items:
            if "slow" in item.keywords:
                item.add_marker(skip_slow)


@pytest.fixture(scope="session")
def hub(request):
    h = request.config._hub_instance
    yield h
    h.close()
