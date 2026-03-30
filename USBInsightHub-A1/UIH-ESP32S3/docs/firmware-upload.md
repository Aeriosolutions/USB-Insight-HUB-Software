# Firmware Upload Guide

There are three ways to flash firmware onto the USB Insight Hub, each suited to different situations.

| Method | Hardware needed | Network needed | When to use |
|--------|----------------|----------------|-------------|
| [USB bootloader](#usb-bootloader-upload) | USB cable only | No | Development — fastest edit-compile-flash cycle |
| [OTA (HTTP)](#ota-upload-over-the-air) | None (WiFi) | Yes | Field updates, no physical access needed |
| [JTAG / ESP-Prog](#jtag-upload-esp-prog) | ESP-Prog + J4 header | No | Bricked device, first-time flash, debugging |

All methods use the same firmware binary produced by PlatformIO.

## Building Firmware

```bash
# Default environment (JTAG/serial upload)
pio run -e esp32-s3-uih

# USB bootloader environment (same firmware, different upload protocol)
pio run -e esp32-s3-uih-usb
```

The firmware binary lands in:
- `.pio/build/esp32-s3-uih/firmware.bin`
- `build/firmware/USBInsightHub-A0_esp32-s3-uih_<version>.bin` (renamed copy)

---

## USB Bootloader Upload

Flashes over the same USB cable used for serial communication. No additional
hardware required.

### How it works

1. The upload script enables `reboot_enabled` via the serial API
2. A 1200-baud "touch" triggers the ESP32-S3 ROM bootloader
3. `esptool` flashes all images (bootloader, partitions, boot_app0, firmware)
4. The `FORCE_DOWNLOAD_BOOT` flag is cleared and the chip hard-resets
5. The application boots normally

### Prerequisites

- Hub is running and connected via USB
- PlatformIO installed (`pio` in PATH or at `~/.platformio/penv/bin/pio`)
- `pyserial` available in PlatformIO's Python environment

### Usage

```bash
# Build and flash in one step
pio run -e esp32-s3-uih-usb -t upload

# Build only, then upload separately
pio run -e esp32-s3-uih-usb
pio run -e esp32-s3-uih-usb -t upload
```

The upload automatically:
- Detects the hub by USB VID/PID and product string
- Enters bootloader mode via 1200-baud touch
- Flashes all partition images
- Resets the chip back to the application
- Verifies the hub comes back online

Upload logs are written to `logs/usb-upload-YYYYMMDD-HHMMSS.log`.

### PlatformIO configuration

The `esp32-s3-uih-usb` environment in `platformio.ini`:

```ini
[env:esp32-s3-uih-usb]
extends = env:esp32-s3-uih
upload_protocol = custom
extra_scripts =
    ${env.extra_scripts}
    scripts/usb_upload.py
```

### If the hub is stuck in bootloader

If a previous upload was interrupted and the hub is stuck in ROM bootloader
mode, the upload script detects this and flashes directly without the
1200-baud touch step.

To manually recover without flashing:

```bash
# Find the bootloader port
# (shows as "USB JTAG/serial debug unit" with VID 0x303A)

# Clear FORCE_DOWNLOAD_BOOT and reset
esptool.py --port /dev/cu.usbmodemXXXX --chip esp32s3 \
    --no-stub --after hard_reset \
    write_mem 0x6000812C 0x0 0x1
```

### Implementation

Script: [`scripts/usb_upload.py`](../USBInsightHub-A1/UIH-ESP32S3/scripts/usb_upload.py)

The upload relies on linker wrapping (`-Wl,--wrap=usb_persist_restart`) to
intercept the Arduino core's bootloader entry. The custom implementation in
`Extercomms.cpp` performs a true system reset (`RTC_CNTL_SW_SYS_RST`) instead
of the default `esp_restart()`, which only resets CPUs on ESP32-S3 and leaves
the USB-OTG peripheral in a bad state.

---

## OTA Upload (Over-the-Air)

Uploads firmware via HTTP to the hub's web interface. Requires the hub to be
connected to WiFi.

### Usage

Shell (macOS/Linux):
```bash
# Use defaults (insighthub.local, latest PIO build)
./scripts/ota_upload.sh

# Specify host
./scripts/ota_upload.sh 192.168.1.50

# Specify host and firmware path
./scripts/ota_upload.sh insighthub.local path/to/firmware.bin

# Or use environment variables
UIH_HOST=192.168.1.50 ./scripts/ota_upload.sh
```

Windows:
```cmd
scripts\ota_upload.bat
scripts\ota_upload.bat 192.168.1.50
scripts\ota_upload.bat insighthub.local path\to\firmware.bin
```

### What the scripts do

1. POST the `.bin` file to `http://<host>/rest/uploadFirmware`
2. The hub validates the image, writes it to the OTA partition, and reboots
3. The new firmware becomes active on the next boot

### HTTP status codes

| Code | Meaning |
|------|---------|
| 200 | Upload successful, hub is rebooting |
| 403 | Authentication required (admin credentials needed) |
| 406 | Wrong file type (must be `.bin`) |
| 500 | Internal flash write error |
| 503 | Wrong chip type (not ESP32-S3 firmware) |
| 507 | Firmware too large for OTA partition |

### Programmatic use (curl)

```bash
curl -X POST \
    -F "file=@firmware.bin;filename=firmware.bin" \
    http://insighthub.local/rest/uploadFirmware
```

If security is enabled, add authentication:
```bash
curl -X POST \
    -H "Authorization: Bearer <token>" \
    -F "file=@firmware.bin;filename=firmware.bin" \
    http://insighthub.local/rest/uploadFirmware
```

### Scripts

- [`scripts/ota_upload.sh`](../USBInsightHub-A1/UIH-ESP32S3/scripts/ota_upload.sh) — macOS/Linux
- [`scripts/ota_upload.bat`](../USBInsightHub-A1/UIH-ESP32S3/scripts/ota_upload.bat) — Windows
- [`scripts/OTA_README.md`](../USBInsightHub-A1/UIH-ESP32S3/scripts/OTA_README.md) — Quick reference

---

## JTAG Upload (ESP-Prog)

Uses an external ESP-Prog programmer connected to header J4 on the interface
board. This is the only option when the hub has no working firmware (bricked),
or when you need hardware debugging.

### Usage

```bash
# Default environment uses esptool serial upload via ESP-Prog
pio run -e esp32-s3-uih -t upload
```

The `esp32-s3-uih` environment uses the default `esptool` upload protocol,
which communicates over the ESP-Prog's serial interface connected to J4.

### When to use

- First-time programming of a blank chip
- Recovery from a completely bricked device
- Hardware debugging with GDB (via JTAG)
- When USB CDC is not available (e.g., `ARDUINO_USB_CDC_ON_BOOT=0` and no
  working firmware to enable it)

### Note on USB CDC

`USB_CDC_ON_BOOT` is disabled by default to avoid interfering with the USB
communication with the Enumeration Extraction Agent. The USB bootloader upload
method works without CDC-on-boot because it uses the TinyUSB CDC interface
configured at runtime in `Extercomms.cpp`.

---

## Upload Scripts Summary

| Script | Location | Purpose |
|--------|----------|---------|
| `usb_upload.py` | `scripts/` | PlatformIO custom upload via USB bootloader |
| `ota_upload.sh` | `scripts/` | OTA upload via HTTP (macOS/Linux) |
| `ota_upload.bat` | `scripts/` | OTA upload via HTTP (Windows) |
| `rename_fw.py` | `scripts/` | Post-build: copies firmware to `build/firmware/` with version in filename |
| `build_interface.py` | `scripts/` | Pre-build: compiles the SvelteKit web interface |
| `generate_cert_bundle.py` | `scripts/` | Pre-build: generates SSL certificate bundle |

---

## Troubleshooting

### USB upload: "Hub not found"

- Ensure the hub is connected and running (displays should be active)
- Check that the hub appears as "InsightHUB Controller" in USB device list
- On macOS: `system_profiler SPUSBDataType | grep -A5 InsightHUB`
- The hub uses VID `0x303A`, PID `0x1001`

### USB upload: bootloader doesn't enumerate

- The 1200-baud touch requires `reboot_enabled=1` — the upload script sets
  this automatically, but if the serial API is unresponsive, it will fail
- Try power-cycling the hub (unplug/replug USB)
- Check the upload log in `logs/usb-upload-*.log`

### OTA upload: connection refused

- Verify the hub is connected to WiFi (check the hub's IP in your router)
- Try using the IP address directly instead of `insighthub.local`
- Ensure no firewall is blocking port 80

### After upload: displays are off / dim

- Brightness defaults to 80% on fresh firmware
- If brightness was set to a low value before flashing, it persists in NVS
- Fix via serial: send `{"action":"set","params":{"brightness":80}}`
- Fix via web UI: Settings > Screen > Brightness slider
