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

## Demos

Interactive demo scripts that stream real-time visuals to the hub's displays
using the binary transport protocol. All demos auto-detect the hub and support
`--port` to override. Press Ctrl-C to stop.

All demos support `--mode buffer|sprite|direct` to select the image write mode.
Image data is RLE-compressed by default (~87% size reduction for typical frames).

### Plasma — `demo_plasma.py`

Animated plasma effect with selectable color palettes.

```bash
python demo_plasma.py                          # channel 2, rainbow palette
python demo_plasma.py --palette neon           # hot pink / cyan / green
python demo_plasma.py --palette fire --fps     # red / orange / yellow + FPS
python demo_plasma.py --channels 1,2,3         # all 3 displays
python demo_plasma.py --mode direct            # use direct SPI streaming
```

Palettes: `rainbow`, `neon`, `fire`, `ocean`, `lava`

### Live Graph — `demo_live_graph.py`

Scrolling dual-trace voltage/current graph. Polls each channel's meter data
and renders a real-time graph on the corresponding display.

```bash
python demo_live_graph.py                      # all 3 channels, fixed axes
python demo_live_graph.py --channels 1         # single channel
python demo_live_graph.py --axis min-span      # auto-scale with minimum range
python demo_live_graph.py --axis auto          # tight auto-scale
python demo_live_graph.py --no-rle             # disable RLE compression
python demo_live_graph.py --record graph.gif   # save frames as animated GIF
```

Y-axis modes: `fixed` (default, 0–5.5V / 0–2A), `min-span` (auto with minimum
range to suppress noise), `auto` (tight auto-scale).

### Tear Test — `demo_tear_test.py`

High-contrast scrolling bars designed to make display tearing visible. If the
display updates mid-frame, you'll see a horizontal offset in the sharp
black/white boundary.

```bash
python demo_tear_test.py                       # default settings
python demo_tear_test.py --bar-height 8        # thinner bars
python demo_tear_test.py --speed 4             # faster scroll
```

### Binary Transport Demo — `demo_binary_transport.py`

Showcases the binary protocol with test patterns (solid fills, rainbow bars,
gradient, checkerboard) and echo latency measurement.

```bash
python demo_binary_transport.py                # run all demos
```

## Test Structure

```
hub.py                    — Hub connection class, device discovery, bootloader recovery
conftest.py               — Pytest fixtures, logging, hub auto-detection, CLI options
binary_transport.py       — Binary frame protocol: build/parse frames, image helpers
test_serial_api.py        — Serial API test cases
test_reboot.py            — Restart, uptime, and reboot toggle tests
test_binary_echo.py       — Binary echo command tests
test_binary_image.py      — Binary image streaming tests (buffer, sprite, direct modes)
test_nvs_persistence.py   — NVS config persistence across reboots
test_nvs_upgrade.py       — NVS migration from v1.0.0 to current firmware
demo_plasma.py            — Animated plasma with selectable palettes
demo_live_graph.py        — Real-time voltage/current graph
demo_tear_test.py         — Display tearing detection
demo_binary_transport.py  — Binary protocol test patterns and echo
requirements.txt          — Python dependencies
snapshots/                — Config snapshots from upgrade test runs
```
