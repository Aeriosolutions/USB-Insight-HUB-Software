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
python3 -m venv .venv
source .venv/bin/activate   # or .venv\Scripts\activate on Windows
pip install -r requirements.txt
```

## Running Tests

The hub is auto-detected by USB VID/PID. Pass `--port` to override.

```bash
# Auto-detect hub, verbose output
pytest -v

# Explicit serial port
pytest -v --port /dev/cu.usbmodemXXX

# Run only channel query tests
pytest -v -k channel
```

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

**Total: 18 tests**

## Safety

All tests that modify state (power, fwdLimit, filterType) save the original
value before testing and restore it in a fixture teardown, even if the test
fails.

## Test Structure

```
conftest.py          — Hub connection class, auto-detection, pytest fixtures
test_serial_api.py   — Serial API test cases
requirements.txt     — Python dependencies
```
