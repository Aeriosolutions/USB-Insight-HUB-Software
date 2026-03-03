"""Integration test for NVS config migration across firmware upgrades.

Verifies that user settings survive an OTA firmware upgrade from v1.0.0
(blob-based NVS) to the current firmware (key-value NVS), and that new
config fields introduced in the current firmware receive correct defaults.

This test is destructive: it flashes old firmware, changes settings, then
flashes the current firmware. Run it manually before releases — not in CI.

Covers GitHub issue #10.

Prerequisites:
    - Hub connected via USB serial (for config reads/writes)
    - Hub connected to WiFi (for OTA upload)
    - v1.0.0 firmware binary available (checked into build/firmware/)
    - Current firmware binary already built (pio run)

Usage:
    .venv/bin/pytest test_nvs_upgrade.py -v -s \\
        --host insighthub.local \\
        --old-firmware build/firmware/USBInsightHub-A0_esp32-s3-uih_1-0-0.bin

    # Or with explicit new firmware path:
    .venv/bin/pytest test_nvs_upgrade.py -v -s \\
        --host 192.168.4.1 \\
        --old-firmware build/firmware/USBInsightHub-A0_esp32-s3-uih_1-0-0.bin \\
        --new-firmware .pio/build/esp32-s3-uih/firmware.bin
"""

import json
import logging
import time
from pathlib import Path

import pytest
import requests

log = logging.getLogger("hub")

SNAPSHOTS_DIR = Path(__file__).parent / "snapshots"


# ── OTA helpers ─────────────────────────────────────────────────

DEFAULT_ADMIN_USER = "admin"
DEFAULT_ADMIN_PASS = "admin"
OTA_TIMEOUT_S = 120
REBOOT_WAIT_S = 15


def get_auth_token(host, username=DEFAULT_ADMIN_USER, password=DEFAULT_ADMIN_PASS):
    """Authenticate with the hub's REST API and return a JWT bearer token.

    Returns None if security is disabled (FT_SECURITY=0).
    """
    url = f"http://{host}/rest/signIn"
    try:
        resp = requests.post(url, json={"username": username, "password": password}, timeout=10)
        if resp.status_code == 200 and "access_token" in resp.text:
            return resp.json()["access_token"]
    except Exception:
        pass
    log.info("Security disabled or signIn unavailable — proceeding without auth")
    return None


def ota_upload(host, firmware_path, token=None):
    """Upload firmware via OTA and wait for the hub to reboot."""
    url = f"http://{host}/rest/uploadFirmware"
    fw_path = Path(firmware_path)
    assert fw_path.exists(), f"Firmware not found: {fw_path}"

    size = fw_path.stat().st_size
    log.info("OTA upload: %s (%d bytes) → %s", fw_path.name, size, host)

    headers = {}
    if token:
        headers["Authorization"] = f"Bearer {token}"

    with open(fw_path, "rb") as f:
        resp = requests.post(
            url,
            files={"file": (fw_path.name, f, "application/octet-stream")},
            headers=headers,
            timeout=OTA_TIMEOUT_S,
        )

    if resp.status_code != 200:
        raise RuntimeError(
            f"OTA upload failed: HTTP {resp.status_code} — {resp.text}"
        )
    log.info("OTA upload accepted — hub is rebooting")


def save_snapshot(name, data):
    """Save a config snapshot to a JSON file for post-mortem analysis."""
    SNAPSHOTS_DIR.mkdir(parents=True, exist_ok=True)
    path = SNAPSHOTS_DIR / f"{name}.json"
    with open(path, "w") as f:
        json.dump(data, f, indent=2)
    log.info("Saved snapshot: %s", path)
    return path


def save_nvs_snapshot(hub, name):
    """Dump the NVS partition to a binary file via bootloader + esptool."""
    SNAPSHOTS_DIR.mkdir(parents=True, exist_ok=True)
    path = SNAPSHOTS_DIR / f"{name}.bin"
    hub.dump_nvs(path)
    return path


# ── Config capture ──────────────────────────────────────────────

def snapshot_full_config(hub):
    """Capture all config + channel fields via serial API."""
    snap = {}

    # Global config
    data = hub.get("config")
    assert data is not None, "get('config') returned None"
    snap["config"] = data

    # Per-channel extended data
    for ch_idx in range(1, 4):
        ch = f"CH{ch_idx}"
        data = hub.get(f"{ch}_all")
        assert data is not None and ch in data, f"get('{ch}_all') failed"
        snap[ch] = data[ch]

    # State (for version, uptime)
    state = hub.get("state")
    if state:
        snap["state"] = state

    return snap


# ── v1.0.0 config fields (no reboot_enabled, no uptime) ────────
# These are the fields we can set on v1.0.0 via its serial API and
# expect to survive migration to the new firmware.
V1_TEST_CONFIG = {
    # Global config (set action params)
    "filterType": "moving_avg",   # default: "median"
    "refreshRate": "1.0s",        # default: "0.5s"
    "hubMode": "usb2",            # default: "usb2&3"
    "brightness": 50,             # default: 800 (raw PWM on v1.0.0)
    # NOTE: v1.0.0 stores brightness as raw 10-100 and returns it as-is.
    # The current firmware stores brightness as 10-bit PWM internally
    # (set converts %, get converts back). The legacy migration copies the
    # raw value, so brightness=50 on v1.0.0 becomes PWM=50 on the new
    # firmware, which the getter reports as ~5%. This is a known migration
    # bug — this test should fail until it's fixed.
}

V1_TEST_CHANNEL_CONFIG = {
    # Per-channel (set via CH1/CH2/CH3)
    "fwdLimit": 500,    # default: 1000
    "backLimit": 50,    # default: 20
    "startup_tmr": 42,  # defaults: 10/20/30
}

# Fields that are NEW in the current firmware and should get defaults
# when upgrading from v1.0.0 (the key won't exist in NVS).
NEW_FIELDS_EXPECTED_DEFAULTS = {
    "reboot_enabled": 0,   # DISABLE
}


# ── Tests ───────────────────────────────────────────────────────


@pytest.mark.slow
class TestNvsUpgradeMigration:
    """Full upgrade path: v1.0.0 → current firmware."""

    @pytest.fixture(autouse=True)
    def _require_host(self, request):
        """Skip if --host not provided (OTA requires WiFi)."""
        self.host = request.config.getoption("--host", default=None)
        if not self.host:
            pytest.skip("--host required for OTA upgrade tests")
        self.old_fw = request.config.getoption("--old-firmware")
        self.new_fw = request.config.getoption("--new-firmware")

    def _ota_flash_and_reconnect(self, hub, firmware_path, label):
        """Flash firmware via OTA, wait, reconnect serial."""
        token = get_auth_token(self.host)
        ota_upload(self.host, firmware_path, token)
        time.sleep(REBOOT_WAIT_S)
        hub.reconnect(timeout=45.0)
        log.info("Reconnected after %s OTA", label)

    def test_upgrade_preserves_settings(self, hub):
        """Settings changed on v1.0.0 survive migration to current firmware.

        Steps:
            1. OTA flash v1.0.0
            2. Snapshot defaults (reference)
            3. Change config to non-default values
            4. Snapshot modified config (reference)
            5. OTA flash current firmware (triggers legacy migration)
            6. Verify old settings survived
            7. Verify new fields have correct defaults
        """
        assert Path(self.old_fw).exists(), f"Old firmware not found: {self.old_fw}"
        assert Path(self.new_fw).exists(), (
            f"New firmware not found: {self.new_fw}\n"
            f"Run 'pio run -e esp32-s3-uih' first."
        )

        # ── Step 1: Flash v1.0.0 ───────────────────────────────
        log.info("=== Step 1: Flashing v1.0.0 ===")
        self._ota_flash_and_reconnect(hub, self.old_fw, "v1.0.0")

        # ── Step 2: Snapshot v1.0.0 defaults ────────────────────
        log.info("=== Step 2: Capturing v1.0.0 defaults ===")
        snap_defaults = snapshot_full_config(hub)
        save_snapshot("v1.0.0-defaults", snap_defaults)

        # ── Step 3: Change settings ─────────────────────────────
        log.info("=== Step 3: Setting non-default config ===")

        ok = hub.set(V1_TEST_CONFIG)
        assert ok, f"Failed to set global config: {V1_TEST_CONFIG}"

        for ch_idx in range(1, 4):
            ch = f"CH{ch_idx}"
            ok = hub.set({ch: V1_TEST_CHANNEL_CONFIG})
            assert ok, f"Failed to set {ch} config: {V1_TEST_CHANNEL_CONFIG}"

        # Wait for auto-save (100ms task + margin)
        time.sleep(1.0)

        # ── Step 4: Snapshot modified config ────────────────────
        log.info("=== Step 4: Capturing v1.0.0 modified config ===")
        snap_modified = snapshot_full_config(hub)
        save_snapshot("v1.0.0-modified", snap_modified)

        # Verify changes took effect on v1.0.0
        for key, val in V1_TEST_CONFIG.items():
            actual = snap_modified["config"].get(key)
            assert actual == val, (
                f"v1.0.0 set failed: {key} = {actual!r}, expected {val!r}"
            )

        for ch_idx in range(1, 4):
            ch = f"CH{ch_idx}"
            for key, val in V1_TEST_CHANNEL_CONFIG.items():
                actual = snap_modified[ch].get(key)
                assert actual == val, (
                    f"v1.0.0 set failed: {ch}.{key} = {actual!r}, expected {val!r}"
                )

        # ── Step 5: Flash current firmware ──────────────────────
        log.info("=== Step 5: Flashing current firmware (triggers migration) ===")
        self._ota_flash_and_reconnect(hub, self.new_fw, "current")

        # ── Step 6: Snapshot post-upgrade config ────────────────
        log.info("=== Step 6: Capturing post-upgrade config ===")
        snap_upgraded = snapshot_full_config(hub)
        save_snapshot("post-upgrade", snap_upgraded)

        # Dump NVS binary (requires reboot_enabled on current firmware).
        # This enables reverting to this exact NVS state if needed.
        hub.set({"reboot_enabled": 1})
        time.sleep(0.5)
        save_nvs_snapshot(hub, "post-upgrade-nvs")

        # ── Step 7: Verify old settings survived ────────────────
        log.info("=== Step 7: Verifying settings survived migration ===")
        mismatches = []

        for key, val in V1_TEST_CONFIG.items():
            actual = snap_upgraded["config"].get(key)
            if actual != val:
                mismatches.append(
                    f"  config.{key}: expected {val!r} (set on v1.0.0), got {actual!r}"
                )

        for ch_idx in range(1, 4):
            ch = f"CH{ch_idx}"
            for key, val in V1_TEST_CHANNEL_CONFIG.items():
                actual = snap_upgraded[ch].get(key)
                if actual != val:
                    mismatches.append(
                        f"  {ch}.{key}: expected {val!r} (set on v1.0.0), got {actual!r}"
                    )

        if mismatches:
            pytest.fail(
                "Settings lost during v1.0.0 → current migration:\n"
                + "\n".join(mismatches)
            )

        # ── Step 8: Verify new fields have defaults ─────────────
        log.info("=== Step 8: Verifying new field defaults ===")
        new_field_issues = []

        for key, expected_default in NEW_FIELDS_EXPECTED_DEFAULTS.items():
            actual = snap_upgraded["config"].get(key)
            if actual is None:
                new_field_issues.append(
                    f"  config.{key}: missing from response (expected {expected_default!r})"
                )
            elif actual != expected_default:
                new_field_issues.append(
                    f"  config.{key}: expected default {expected_default!r}, got {actual!r}"
                )

        if new_field_issues:
            pytest.fail(
                "New fields don't have correct defaults after migration:\n"
                + "\n".join(new_field_issues)
            )

        # ── Step 9: Verify uptime is available (new feature) ────
        data = hub.get("uptime")
        assert data is not None and "uptime" in data, "uptime not available after upgrade"
        assert data["uptime"] > 0, f"uptime should be positive, got {data['uptime']}"

        log.info("=== Upgrade test PASSED ===")
