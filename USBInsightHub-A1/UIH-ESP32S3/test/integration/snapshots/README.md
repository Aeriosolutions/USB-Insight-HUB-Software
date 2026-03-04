# NVS Upgrade Snapshots

Config snapshots captured during `test_nvs_upgrade.py` runs. Two types of
snapshot are saved:

- **JSON** (`.json`) — human-readable config values read via the serial API
- **Binary** (`.bin`) — raw NVS partition dump read via esptool in bootloader mode

## Files

| File | Description |
|------|-------------|
| `v1.0.0-defaults.json` | Config immediately after flashing v1.0.0 (factory defaults) |
| `v1.0.0-modified.json` | Config after setting non-default values on v1.0.0 |
| `post-upgrade.json` | Config after OTA to the current firmware (post-migration) |
| `post-upgrade-nvs.bin` | Raw NVS partition after migration (for reverting) |

## How they're created

The upgrade test (`test_nvs_upgrade.py`) runs these steps:

1. OTA flash v1.0.0 firmware
2. Read all config via serial API &rarr; `v1.0.0-defaults.json`
3. Set test values (filterType, refreshRate, hubMode, brightness, per-channel limits)
4. Read config again &rarr; `v1.0.0-modified.json`
5. OTA flash current firmware (triggers legacy blob &rarr; key-value migration)
6. Read config again &rarr; `post-upgrade.json`
7. Enable `reboot_enabled`, enter bootloader via 1200-baud touch, dump NVS
   partition via esptool `read_flash` &rarr; `post-upgrade-nvs.bin`
8. Exit bootloader (hard reset back to app)

The test then asserts that values from step 3 survived in step 6, and that
new fields (e.g. `reboot_enabled`) have their expected defaults.

## Capturing and reverting NVS snapshots

The hub must be in ROM bootloader mode for these operations. To enter
bootloader: set `reboot_enabled=1` (via serial API, web UI, or on-board
menu), then perform a 1200-baud touch on the serial port.

### Using the helper scripts

```bash
# Enter bootloader first (from a serial terminal or Python):
#   {"action":"set","params":{"reboot_enabled":1}}
#   then 1200-baud touch

# List ports to find the bootloader device
python3 -m serial.tools.list_ports -v

# Dump current NVS (port is required — no auto-detect)
./scripts/nvs_dump.sh /dev/cu.usbmodemXXX
./scripts/nvs_dump.sh /dev/cu.usbmodemXXX my-snapshot.bin

# Restore a previous snapshot (reboots hub into app automatically)
./scripts/nvs_restore.sh /dev/cu.usbmodemXXX my-snapshot.bin
```

### Using the test framework (Python)

```python
from hub import Hub, find_hub

hub = Hub(find_hub())
hub.set({"reboot_enabled": 1})   # enable bootloader entry
hub.dump_nvs("my-snapshot.bin")  # enters bootloader, dumps, returns to app
hub.restore_nvs("my-snapshot.bin")  # enters bootloader, writes, reboots
```

### Using esptool directly

```bash
# Dump NVS (stays in bootloader after reading)
esptool.py --port /dev/cu.usbmodemXXX --chip esp32s3 --no-stub \
    --after no_reset \
    read_flash 0x9000 0x5000 my-snapshot.bin

# Restore NVS (hard resets into app after writing)
esptool.py --port /dev/cu.usbmodemXXX --chip esp32s3 --no-stub \
    --after hard_reset \
    write_flash 0x9000 my-snapshot.bin

# Exit bootloader without writing anything
esptool.py --port /dev/cu.usbmodemXXX --chip esp32s3 --no-stub \
    --after hard_reset \
    write_mem 0x6000812C 0x0 0x1
```

## NVS partition layout

From `partition_uih_8MB.csv`:

| Name | Offset | Size |
|------|--------|------|
| nvs | 0x9000 | 0x5000 (20 KB) |

## Future work

A serial API command for NVS dump/restore would eliminate the need to enter
bootloader mode, making this accessible to end users without esptool. The
binary format would be identical — same NVS partition bytes, different
transport.
