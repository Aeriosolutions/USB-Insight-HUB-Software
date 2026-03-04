"""Integration tests for NVS config persistence across reboots.

Verifies that configuration changes survive a device restart, covering
the issue described in GitHub #10 (NVS config blob migration resets all
settings when adding new fields).

These tests are slow (each reboot cycle takes ~10-30s) and are intended
to be run before releases, not on every commit.

Usage:
    .venv/bin/pytest test_nvs_persistence.py -v
    .venv/bin/pytest test_nvs_persistence.py -v --port /dev/cu.usbmodemXXX
    .venv/bin/pytest test_nvs_persistence.py -v --quick   # skip reboot tests
"""

import time

import pytest


# ── Helpers ─────────────────────────────────────────────────────


# Config fields returned by get("config") and their non-default test values.
CONFIG_FIELDS = {
    "filterType":     ("moving_avg", str),   # default is "median"
    "refreshRate":    ("1.0s",       str),   # default is "0.5s"
    "brightness":     (50,           int),   # API accepts 5-100 (%)
    "reboot_enabled": (0,            int),   # default is 0 (DISABLE)
}

# Per-channel config fields (via CHx_all) with non-default test values.
CHANNEL_FIELDS = {
    "fwdLimit":    (500,  int),   # default 1000
    "backLimit":   (50,   int),   # default 20
    "startup_tmr": (42,   int),   # defaults 10/20/30
}


def snapshot_config(hub):
    """Capture all persistent config values from the device."""
    snap = {}
    data = hub.get("config")
    assert data is not None, "get('config') returned None"
    for key in CONFIG_FIELDS:
        snap[key] = data.get(key)

    for ch_idx in range(1, 4):
        ch = f"CH{ch_idx}"
        data = hub.get(f"{ch}_all")
        assert data is not None and ch in data, f"get('{ch}_all') failed"
        for key in CHANNEL_FIELDS:
            snap[f"{ch}.{key}"] = data[ch].get(key)

    return snap


def apply_test_config(hub):
    """Set all config fields to non-default test values."""
    params = {}
    for key, (val, _) in CONFIG_FIELDS.items():
        params[key] = val
    ok = hub.set(params)
    assert ok, f"set({params}) failed"

    for ch_idx in range(1, 4):
        ch = f"CH{ch_idx}"
        ch_params = {}
        for key, (val, _) in CHANNEL_FIELDS.items():
            ch_params[key] = val
        ok = hub.set({ch: ch_params})
        assert ok, f"set({ch}: {ch_params}) failed"

    # Allow auto-save task to persist (runs every 100ms)
    time.sleep(0.5)


def expected_test_snapshot():
    """Build the expected snapshot after apply_test_config."""
    snap = {}
    for key, (val, _) in CONFIG_FIELDS.items():
        snap[key] = val
    for ch_idx in range(1, 4):
        ch = f"CH{ch_idx}"
        for key, (val, _) in CHANNEL_FIELDS.items():
            snap[f"{ch}.{key}"] = val
    return snap


# ── Tests ───────────────────────────────────────────────────────


@pytest.mark.slow
@pytest.mark.timeout(60)
class TestNvsPersistenceAcrossReboot:
    """Config values set via serial API survive a software restart."""

    @pytest.fixture(autouse=True)
    def _save_and_restore(self, hub):
        """Capture config before test, restore after."""
        self._orig = snapshot_config(hub)
        yield
        # Restore original values
        restore_global = {}
        for key in CONFIG_FIELDS:
            if self._orig[key] is not None:
                restore_global[key] = self._orig[key]
        if restore_global:
            hub.set(restore_global)

        for ch_idx in range(1, 4):
            ch = f"CH{ch_idx}"
            ch_params = {}
            for key in CHANNEL_FIELDS:
                orig_val = self._orig.get(f"{ch}.{key}")
                if orig_val is not None:
                    ch_params[key] = orig_val
            if ch_params:
                hub.set({ch: ch_params})

        time.sleep(0.5)  # let auto-save persist the restore

    def test_config_survives_restart(self, hub):
        """All config fields retain their values after a software restart."""
        # 1. Apply non-default values
        apply_test_config(hub)

        # 2. Verify they took effect before reboot
        pre_reboot = snapshot_config(hub)
        expected = expected_test_snapshot()
        for key, exp_val in expected.items():
            assert pre_reboot[key] == exp_val, (
                f"Pre-reboot: {key} = {pre_reboot[key]!r}, expected {exp_val!r}"
            )

        # 3. Restart the device
        hub.send({"action": "restart", "params": {"immediate": True}})
        hub.reconnect(timeout=30.0)

        # 4. Read back and compare
        post_reboot = snapshot_config(hub)
        mismatches = []
        for key, exp_val in expected.items():
            actual = post_reboot[key]
            if actual != exp_val:
                mismatches.append(f"  {key}: expected {exp_val!r}, got {actual!r}")

        assert not mismatches, (
            "Config values changed after reboot:\n" + "\n".join(mismatches)
        )


@pytest.mark.slow
@pytest.mark.timeout(60)
class TestNvsIndividualFieldPersistence:
    """Each settable config field individually survives a reboot.

    Parametrized so failures pinpoint the exact field that didn't persist.
    """

    @pytest.fixture(autouse=True)
    def _save_and_restore_single(self, hub, request):
        """Capture and restore just the field under test."""
        self._field = None
        yield
        # Restore handled per-test below

    @pytest.mark.parametrize("field,test_val", [
        ("filterType", "moving_avg"),
        ("refreshRate", "1.0s"),
        ("brightness", 50),
    ])
    def test_global_field(self, hub, field, test_val):
        """Global config field persists across reboot."""
        # Save original
        orig_data = hub.get(field)
        orig_val = orig_data.get(field) if orig_data else None

        try:
            # Set test value
            assert hub.set({field: test_val}), f"set({field}={test_val}) failed"
            time.sleep(0.5)

            # Verify pre-reboot
            data = hub.get(field)
            assert data.get(field) == test_val, (
                f"Pre-reboot: {field} = {data.get(field)!r}, expected {test_val!r}"
            )

            # Reboot
            hub.send({"action": "restart", "params": {"immediate": True}})
            hub.reconnect(timeout=30.0)

            # Verify post-reboot
            data = hub.get(field)
            assert data.get(field) == test_val, (
                f"Post-reboot: {field} = {data.get(field)!r}, expected {test_val!r}"
            )
        finally:
            # Restore
            if orig_val is not None:
                hub.set({field: orig_val})
                time.sleep(0.3)

    @pytest.mark.parametrize("field,test_val", [
        ("fwdLimit", 500),
        ("backLimit", 50),
        ("startup_tmr", 42),
    ])
    def test_channel_field(self, hub, field, test_val):
        """Per-channel config field (CH1) persists across reboot."""
        ch = "CH1"

        # Save original
        orig_data = hub.get(f"{ch}_all")
        orig_val = orig_data[ch].get(field) if orig_data and ch in orig_data else None

        try:
            # Set test value
            assert hub.set({ch: {field: test_val}}), f"set({ch}.{field}={test_val}) failed"
            time.sleep(0.5)

            # Verify pre-reboot
            data = hub.get(f"{ch}_all")
            assert data[ch].get(field) == test_val, (
                f"Pre-reboot: {ch}.{field} = {data[ch].get(field)!r}, expected {test_val!r}"
            )

            # Reboot
            hub.send({"action": "restart", "params": {"immediate": True}})
            hub.reconnect(timeout=30.0)

            # Verify post-reboot
            data = hub.get(f"{ch}_all")
            assert data[ch].get(field) == test_val, (
                f"Post-reboot: {ch}.{field} = {data[ch].get(field)!r}, expected {test_val!r}"
            )
        finally:
            # Restore
            if orig_val is not None:
                hub.set({ch: {field: orig_val}})
                time.sleep(0.3)


class TestNvsDefaults:
    """Verify factory default values match expected spec (no reboot needed)."""

    def test_default_config_keys_present(self, hub):
        """get('config') returns all expected keys."""
        data = hub.get("config")
        assert data is not None
        expected_keys = [
            "startUpmode", "wifi_enabled", "hubMode",
            "filterType", "refreshRate", "brightness", "reboot_enabled",
        ]
        for key in expected_keys:
            assert key in data, f"config missing key: {key}"

    def test_channel_all_keys_present(self, hub):
        """get('CHx_all') returns config fields for each channel."""
        for ch_idx in range(1, 4):
            ch = f"CH{ch_idx}"
            data = hub.get(f"{ch}_all")
            assert data is not None and ch in data, f"{ch}_all query failed"
            for key in ("fwdLimit", "backLimit", "startup_tmr", "ilim"):
                assert key in data[ch], f"{ch}_all missing {key}"

    def test_brightness_in_range(self, hub):
        """Brightness percentage is within valid API range."""
        data = hub.get("brightness")
        val = data.get("brightness")
        assert isinstance(val, int), f"brightness is {type(val)}, expected int"
        assert 0 <= val <= 100, f"brightness {val} out of range [0, 100]"

    def test_fwd_limit_in_range(self, hub):
        """Forward current limit is within valid range for all channels."""
        for ch_idx in range(1, 4):
            ch = f"CH{ch_idx}"
            data = hub.get(f"{ch}_all")
            val = data[ch]["fwdLimit"]
            assert 100 <= val <= 2000, f"{ch} fwdLimit {val} out of range [100,2000]"

    def test_back_limit_in_range(self, hub):
        """Back current limit is within valid range for all channels."""
        for ch_idx in range(1, 4):
            ch = f"CH{ch_idx}"
            data = hub.get(f"{ch}_all")
            val = data[ch]["backLimit"]
            assert 1 <= val <= 200, f"{ch} backLimit {val} out of range [1,200]"

    def test_uptime_available(self, hub):
        """Uptime (millis) is reported and positive."""
        data = hub.get("uptime")
        assert data is not None
        val = data.get("uptime")
        assert isinstance(val, int) and val > 0, f"uptime = {val!r}"
