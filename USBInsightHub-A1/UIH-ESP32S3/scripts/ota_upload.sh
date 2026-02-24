#!/usr/bin/env bash
# Upload firmware to USB Insight Hub via OTA (HTTP)
# Usage: ota_upload.sh [host] [firmware.bin]
# Environment variables UIH_HOST and UIH_FIRMWARE override the defaults.
set -euo pipefail

HOST="${1:-${UIH_HOST:-insighthub.local}}"
FIRMWARE="${2:-${UIH_FIRMWARE:-$(dirname "$0")/../.pio/build/esp32-s3-uih/firmware.bin}}"

if [ ! -f "$FIRMWARE" ]; then
    echo "error: firmware not found: $FIRMWARE" >&2
    echo "usage: $0 [host] [firmware.bin]" >&2
    exit 1
fi

SIZE=$(stat -f%z "$FIRMWARE" 2>/dev/null || stat -c%s "$FIRMWARE" 2>/dev/null)
echo "Uploading $(basename "$FIRMWARE") (${SIZE} bytes) to ${HOST}..."

# Upload with progress
HTTP_CODE=$(curl -s -w '%{http_code}' -o /dev/null \
    --connect-timeout 5 \
    --max-time 120 \
    -X POST \
    -F "file=@${FIRMWARE};filename=$(basename "$FIRMWARE")" \
    "http://${HOST}/rest/uploadFirmware")

if [ "$HTTP_CODE" = "200" ]; then
    echo "Upload successful — hub is rebooting."
    echo "Wait ~10s for it to come back online."
else
    echo "error: upload failed (HTTP ${HTTP_CODE})" >&2
    case "$HTTP_CODE" in
        403) echo "  → Forbidden (admin auth required)" >&2 ;;
        406) echo "  → Not acceptable (wrong file type, must be .bin)" >&2 ;;
        500) echo "  → Internal error (flash write failed)" >&2 ;;
        503) echo "  → Wrong chip type (not ESP32-S3 firmware)" >&2 ;;
        507) echo "  → Insufficient storage (firmware too large)" >&2 ;;
        000) echo "  → Connection failed (is the hub on the network?)" >&2 ;;
    esac
    exit 1
fi
