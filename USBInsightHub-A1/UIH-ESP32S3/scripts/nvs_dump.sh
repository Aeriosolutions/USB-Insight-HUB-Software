#!/usr/bin/env bash
# Dump the NVS partition from an Insight Hub to a binary file.
#
# The hub must be in ROM bootloader mode before running this script.
# Enter bootloader by enabling reboot_enabled=1 via the serial API or
# web UI, then performing a 1200-baud touch on the serial port.
#
# Usage:
#   ./scripts/nvs_dump.sh /dev/cu.usbmodemXXX
#   ./scripts/nvs_dump.sh /dev/cu.usbmodemXXX my-snapshot.bin
#
# After dumping, the hub stays in bootloader mode (--after no_reset).
# Use nvs_restore.sh or esptool hard_reset to return to the app.
set -euo pipefail

NVS_OFFSET=0x9000
NVS_SIZE=0x5000

PORT="${1:-}"
OUTPUT="${2:-nvs_snapshot.bin}"

if [ -z "$PORT" ]; then
    echo "usage: $0 <bootloader-port> [output.bin]" >&2
    echo "  Port is required — auto-detect is intentionally disabled to avoid" >&2
    echo "  flashing the wrong device. List ports with: python3 -m serial.tools.list_ports -v" >&2
    exit 1
fi

echo "Reading NVS partition (${NVS_OFFSET}, ${NVS_SIZE} bytes) from ${PORT}..."

esptool.py --port "$PORT" --chip esp32s3 --no-stub \
    --after no_reset \
    read_flash "$NVS_OFFSET" "$NVS_SIZE" "$OUTPUT"

SIZE=$(stat -f%z "$OUTPUT" 2>/dev/null || stat -c%s "$OUTPUT" 2>/dev/null)
echo "NVS dump saved to ${OUTPUT} (${SIZE} bytes)"
echo ""
echo "Hub is still in bootloader mode. To return to the app:"
echo "  ./scripts/nvs_restore.sh ${PORT} ${OUTPUT}   # restore and reboot"
echo "  # or just reboot:"
echo "  esptool.py --port ${PORT} --chip esp32s3 --no-stub --after hard_reset write_mem 0x6000812C 0x0 0x1"
