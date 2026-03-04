#!/usr/bin/env bash
# Restore the NVS partition on an Insight Hub from a binary file.
#
# The hub must be in ROM bootloader mode before running this script.
# Enter bootloader by enabling reboot_enabled=1 via the serial API or
# web UI, then performing a 1200-baud touch on the serial port.
#
# After writing, the hub is hard-reset back into the application with
# the restored NVS data.
#
# Usage:
#   ./scripts/nvs_restore.sh /dev/cu.usbmodemXXX nvs_snapshot.bin
set -euo pipefail

NVS_OFFSET=0x9000

PORT="${1:-}"
INPUT="${2:-}"

if [ -z "$PORT" ] || [ -z "$INPUT" ]; then
    echo "usage: $0 <bootloader-port> <nvs_snapshot.bin>" >&2
    echo "  Port is required — auto-detect is intentionally disabled to avoid" >&2
    echo "  flashing the wrong device. List ports with: python3 -m serial.tools.list_ports -v" >&2
    exit 1
fi

if [ ! -f "$INPUT" ]; then
    echo "error: file not found: $INPUT" >&2
    exit 1
fi

SIZE=$(stat -f%z "$INPUT" 2>/dev/null || stat -c%s "$INPUT" 2>/dev/null)
echo "Writing NVS partition (${NVS_OFFSET}, ${SIZE} bytes) to ${PORT} from ${INPUT}..."

esptool.py --port "$PORT" --chip esp32s3 --no-stub \
    --after hard_reset \
    write_flash "$NVS_OFFSET" "$INPUT"

echo "NVS restored. Hub is rebooting into the application."
