# USB Insight Hub — Serial Integration Tests

Hardware integration tests for the Insight Hub's JSON serial API. These tests
run against a real hub connected via USB and verify correct behavior of the
serial command interface.

## Prerequisites

- USB Insight Hub connected via USB
- Python 3.10+

## Setup

```bash
cd USBInsightHub-A1/UIH-ESP32S3/test/integration
./setup.sh
```

This creates the Python venv and installs dependencies.

Manual setup (or Windows):
```bash
python3 -m venv .venv  # or py -m venv .venv on Windows
.venv/bin/pip install -r requirements.txt  # or .venv\Scripts\pip on Windows
```

## Running Tests

The hub is auto-detected by USB VID/PID. Pass `--port` to override.

```bash
# Auto-detect hub, verbose output
.venv/bin/pytest -v

# Explicit serial port
.venv/bin/pytest -v --port /dev/cu.usbmodemXXX

# Skip slow tests (reboot, bootloader operations)
.venv/bin/pytest -v --quick

# Run only channel query tests
.venv/bin/pytest -v -k channel
```

### NVS Persistence Tests

Tests that settings survive a reboot. Requires the current firmware with
`restart` support.

```bash
.venv/bin/pytest test_nvs_persistence.py -v -s
```

### NVS Upgrade Migration Test

Tests the v1.0.0 → current firmware migration path via OTA. Requires WiFi
connectivity to the hub.

```bash
.venv/bin/pytest test_nvs_upgrade.py -v -s --host insighthub.local
```

The `--old-firmware` and `--new-firmware` options default to
`build/firmware/USBInsightHub-A0_esp32-s3-uih_1-0-0.bin` and
`.pio/build/esp32-s3-uih/firmware.bin` respectively.

## Troubleshooting

Every run writes a detailed log to `./logs/run-YYYYMMDD-HHMMSS.log` with full
serial TX/RX traffic, timeouts, and JSON parse errors. The log path is printed
at the end of the test summary.

```bash
# Run tests — log is written automatically
pytest -v

# Check the log
cat logs/run-*.log | tail -40
```

To also show the serial traffic live on the console:

```bash
pytest -v --log-cli-level=DEBUG
```

## Test Report

Generate an HTML report with `--html`:

```bash
pytest -v --html=report.html --self-contained-html
```

Generate a plain-text summary of what each test verifies:

```bash
pytest --co -q          # list test names
pytest --co -v          # list test names with class/module structure
```

The tests are self-documenting — every test class and method has a docstring
describing what it verifies. Use `pytest --co -v` to see the full hierarchy.

## What's Tested

| Area | Tests | Description |
|------|-------|-------------|
| **Channel queries** | 4 | Required fields (powerEn, dataEn, voltage, current) on CH1-CH3; extended query |
| **Global config** | 3 | Meta-names (all/config/state); config param presence; startUpmode enum |
| **Enum roundtrip** | 3 | filterType set/get with valid values; invalid enum rejection |
| **Forward limit** | 4 | CH1 fwdLimit set/get across 100-2000 range |
| **Power control** | 2 | CH1 power off/on via serial API |
| **Edge cases** | 2 | Invalid action handling; firstStart auto-clear flag |
| **Uptime** | 4 | Uptime present, in state/all responses, monotonically increasing |
| **Reboot config** | 5 | reboot_enabled in config, default disabled, set/get roundtrip, invalid rejection |
| **Restart command** | 2 | Graceful and immediate restart verified via uptime reset |
| **Reboot toggle** | 2 | 1200-baud touch ignored when disabled; enters bootloader when enabled |
| **NVS persistence** | 13 | Config survives reboot; per-field persistence; defaults validation |
| **NVS upgrade** | 1 | v1.0.0 blob → key-value migration via OTA (includes known brightness bug) |

**Total: 45 tests**

## Safety

All tests that modify state (power, fwdLimit, filterType, reboot_enabled) save
the original value before testing and restore it in a fixture teardown, even if
the test fails.

The reboot toggle test (`test_enabled_enters_bootloader`) puts the hub into ROM
bootloader and recovers it via `usb-device boot`. If recovery fails, power-cycle
the hub.

## Test Structure

```
conftest.py               — Pytest fixtures, hub connection, CLI options
hub.py                    — Hub class and device discovery (reusable outside pytest)
test_serial_api.py        — Serial API test cases
test_reboot.py            — Restart, uptime, and reboot toggle tests
test_nvs_persistence.py   — NVS config persistence across reboots
test_nvs_upgrade.py       — NVS migration from v1.0.0 to current firmware
requirements.txt          — Python dependencies
snapshots/                — Config snapshots from upgrade test runs
```
