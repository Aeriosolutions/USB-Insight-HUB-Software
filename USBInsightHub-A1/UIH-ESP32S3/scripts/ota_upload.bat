@echo off
REM Upload firmware to USB Insight Hub via OTA (HTTP)
REM Usage: ota_upload.bat [host] [firmware.bin]
REM Environment variables UIH_HOST and UIH_FIRMWARE override the defaults.
REM Requires: curl (included in Windows 10+)

setlocal enabledelayedexpansion

set "HOST=%~1"
if "%HOST%"=="" if defined UIH_HOST (set "HOST=%UIH_HOST%") else (set "HOST=insighthub.local")

set "FIRMWARE=%~2"
if "%FIRMWARE%"=="" if defined UIH_FIRMWARE (set "FIRMWARE=%UIH_FIRMWARE%") else (set "FIRMWARE=%~dp0..\.pio\build\esp32-s3-uih\firmware.bin")

if not exist "%FIRMWARE%" (
    echo error: firmware not found: %FIRMWARE% >&2
    echo usage: %~nx0 [host] [firmware.bin] >&2
    exit /b 1
)

for %%F in ("%FIRMWARE%") do set "FNAME=%%~nxF" & set "FSIZE=%%~zF"
echo Uploading %FNAME% (%FSIZE% bytes) to %HOST%...

curl -s -w "%%{http_code}" -o NUL ^
    --connect-timeout 5 ^
    --max-time 120 ^
    -X POST ^
    -F "file=@%FIRMWARE%;filename=%FNAME%" ^
    "http://%HOST%/rest/uploadFirmware" > "%TEMP%\ota_http_code.tmp"

set /p HTTP_CODE=<"%TEMP%\ota_http_code.tmp"
del "%TEMP%\ota_http_code.tmp" 2>nul

if "%HTTP_CODE%"=="200" (
    echo Upload successful — hub is rebooting.
    echo Wait ~10s for it to come back online.
    exit /b 0
)

echo error: upload failed (HTTP %HTTP_CODE%) >&2
if "%HTTP_CODE%"=="403" echo   → Forbidden (admin auth required) >&2
if "%HTTP_CODE%"=="406" echo   → Not acceptable (wrong file type, must be .bin) >&2
if "%HTTP_CODE%"=="500" echo   → Internal error (flash write failed) >&2
if "%HTTP_CODE%"=="503" echo   → Wrong chip type (not ESP32-S3 firmware) >&2
if "%HTTP_CODE%"=="507" echo   → Insufficient storage (firmware too large) >&2
if "%HTTP_CODE%"=="000" echo   → Connection failed (is the hub on the network?) >&2
exit /b 1
