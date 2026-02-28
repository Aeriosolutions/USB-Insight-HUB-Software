# Use defaults (insighthub.local, PIO build output)
./scripts/ota_upload.sh

# Override via env vars
UIH_HOST=192.168.1.50 ./scripts/ota_upload.sh

# Override via positional args
./scripts/ota_upload.sh myhub.local path/to/firmware.bin