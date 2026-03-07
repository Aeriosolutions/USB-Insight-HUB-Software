"""Integration tests for USB Insight Hub serial API.

Tests cover the existing serial API functionality: channel queries,
global config, enum roundtrip, forward current limits, power control,
and edge cases.

Requires a real hub connected via USB.

Usage:
    pytest test/integration/ -v                              # auto-detect hub
    pytest test/integration/ -v --port /dev/cu.usbmodemXXX   # explicit port
"""

import time

import pytest


# ── Channel queries ──────────────────────────────────────────


class TestChannelQuery:
    """Query each channel returns expected fields."""

    @pytest.mark.parametrize("ch", ["CH1", "CH2", "CH3"])
    def test_channel_has_required_fields(self, hub, ch):
        """Each channel reports powerEn, dataEn, voltage, and current."""
        data = hub.get(ch)
        assert data is not None and ch in data
        state = data[ch]
        for field in ("powerEn", "dataEn", "voltage", "current"):
            assert field in state, f"{ch} missing {field}"

    def test_extended_query(self, hub):
        """CH1_all returns extra fields beyond CH1."""
        data = hub.get("CH1_all")
        assert data is not None and "CH1" in data
        state = data["CH1"]
        for field in ("ilim", "fwdLimit", "backLimit", "startup_tmr"):
            assert field in state, f"CH1_all missing {field}"


# ── Global config ────────────────────────────────────────────


class TestGlobalConfig:
    def test_meta_names(self, hub):
        """Meta-names 'all', 'config', 'state' return data."""
        for meta in ["all", "config", "state"]:
            data = hub.get(meta)
            assert data is not None and len(data) > 0, f"get '{meta}' empty"

    def test_config_params(self, hub):
        """Config response includes startUpmode, wifi_enabled, hubMode, brightness."""
        data = hub.get("config")
        assert data is not None
        for key in ("startUpmode", "wifi_enabled", "hubMode", "brightness"):
            assert key in data, f"config missing {key}"

    def test_startup_mode_valid(self, hub):
        """startUpmode is one of the known enum values."""
        data = hub.get("config")
        valid = ("persistance", "on_at_start", "off_at_start", "sequence")
        assert data["startUpmode"] in valid, f"got {data['startUpmode']}"


# ── Set/get roundtrips ───────────────────────────────────────


class TestEnumRoundtrip:
    """Enum parameters accept valid values and reject invalid ones."""

    @pytest.fixture(autouse=True)
    def _save_restore_filter(self, hub):
        data = hub.get("filterType")
        self._orig = data.get("filterType") if data else None
        yield
        if self._orig is not None:
            hub.set({"filterType": self._orig})

    @pytest.mark.parametrize("val", ["moving_avg", "median"])
    def test_filter_type(self, hub, val):
        """filterType set/get roundtrip with valid enum values."""
        hub.set({"filterType": val})
        time.sleep(0.1)
        data = hub.get("filterType")
        assert data.get("filterType") == val

    def test_rejects_invalid(self, hub):
        """Invalid enum value returns 'fail'."""
        resp = hub.send({"action": "set", "params": {"filterType": "bogus"}})
        data = resp.get("data", {}) if resp else {}
        assert data.get("filterType") == "fail"


class TestFwdLimitRoundtrip:
    """Per-channel forward current limit set/get roundtrip."""

    @pytest.fixture(autouse=True)
    def _save_restore_fwd_limit(self, hub):
        data = hub.get("CH1_all")
        self._orig = data["CH1"].get("fwdLimit") if data and "CH1" in data else None
        yield
        if self._orig is not None:
            hub.set({"CH1": {"fwdLimit": self._orig}})

    @pytest.mark.parametrize("val", [100, 500, 1000, 2000])
    def test_roundtrip(self, hub, val):
        """Set CH1 fwdLimit, read back via CH1_all, verify match."""
        hub.set({"CH1": {"fwdLimit": val}})
        time.sleep(0.1)
        data = hub.get("CH1_all")
        assert data["CH1"].get("fwdLimit") == val


# ── Power control ────────────────────────────────────────────


class TestPowerToggle:
    """Power on/off control via serial API on CH1."""

    @pytest.fixture(autouse=True)
    def _save_restore_power(self, hub):
        data = hub.get("CH1")
        self._orig = data["CH1"].get("powerEn") if data and "CH1" in data else True
        yield
        hub.set({"CH1": {"powerEn": "true" if self._orig else "false"}})

    def test_power_off(self, hub):
        """Turning CH1 off sets powerEn to False."""
        hub.set({"CH1": {"powerEn": "false"}})
        time.sleep(0.3)
        data = hub.get("CH1")
        assert data["CH1"]["powerEn"] is False

    def test_power_on(self, hub):
        """Turning CH1 on after off sets powerEn to True."""
        hub.set({"CH1": {"powerEn": "false"}})
        time.sleep(0.3)
        hub.set({"CH1": {"powerEn": "true"}})
        time.sleep(0.3)
        data = hub.get("CH1")
        assert data["CH1"]["powerEn"] is True


# ── Edge cases ───────────────────────────────────────────────


def test_invalid_action(hub):
    """Invalid action returns an error response without hanging."""
    resp = hub.send({"action": "bogus", "params": {}})
    assert resp is not None, "No response for unknown action (firmware should return error)"
    assert resp.get("status") == "error"


def test_first_start_flag(hub):
    """firstStart auto-clears after first read."""
    hub.get("firstStart")  # consume
    data = hub.get("firstStart")
    assert data is not None
    assert data.get("firstStart") is False
