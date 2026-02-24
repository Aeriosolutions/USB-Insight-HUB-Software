"""Integration tests for the brightness uint16_t fix.

Verifies that brightness values in the 50-1000 range roundtrip correctly
(previously truncated via uint8_t) and that out-of-range values are rejected.
"""

import time

import pytest


class TestBrightnessRoundtrip:
    """Verify the uint16_t brightness fix: set/get across 50-1000."""

    @pytest.fixture(autouse=True)
    def _save_restore_brightness(self, hub):
        data = hub.get("brightness")
        self._orig = data.get("brightness") if data else None
        yield
        if self._orig is not None:
            hub.set({"brightness": self._orig})

    @pytest.mark.parametrize("val", [50, 100, 200, 500, 800, 1000])
    def test_roundtrip(self, hub, val):
        """Set brightness to val, read it back, verify it matches."""
        hub.set({"brightness": val})
        time.sleep(0.1)
        data = hub.get("brightness")
        assert data is not None, "get brightness returned None"
        assert data["brightness"] == val

    def test_default_800_roundtrips(self, hub):
        """800 is the factory default — previously truncated to 32 via uint8_t."""
        hub.set({"brightness": 800})
        time.sleep(0.1)
        data = hub.get("brightness")
        assert data["brightness"] == 800


class TestBrightnessOutOfRange:
    """Brightness rejects values outside 50-1000."""

    @pytest.fixture(autouse=True)
    def _save_restore_brightness(self, hub):
        data = hub.get("brightness")
        self._orig = data.get("brightness") if data else None
        yield
        if self._orig is not None:
            hub.set({"brightness": self._orig})

    @pytest.mark.parametrize("val", [10, 49, 1001, 2000])
    def test_rejects_invalid(self, hub, val):
        """Setting brightness outside 50-1000 returns an 'out of range' error."""
        resp = hub.send({"action": "set", "params": {"brightness": val}})
        data = resp.get("data", {}) if resp else {}
        assert "brightness" in data
        assert "out of range" in str(data["brightness"])

    def test_zero_is_ignored(self, hub):
        """ArduinoJson treats 0 as falsy, so brightness=0 is silently skipped."""
        before = hub.get("brightness")
        before_val = before.get("brightness") if before else None
        hub.send({"action": "set", "params": {"brightness": 0}})
        time.sleep(0.1)
        after = hub.get("brightness")
        assert after.get("brightness") == before_val

    def test_value_unchanged_after_invalid(self, hub):
        """Invalid brightness values do not alter the stored brightness."""
        before = hub.get("brightness")
        before_val = before.get("brightness") if before else None

        hub.send({"action": "set", "params": {"brightness": 9999}})
        time.sleep(0.1)

        after = hub.get("brightness")
        after_val = after.get("brightness") if after else None
        assert after_val == before_val
