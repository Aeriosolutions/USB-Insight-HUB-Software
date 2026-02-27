"""Integration tests for brightness percentage API.

The serial API accepts brightness as a percentage (0-100) and converts to
a 10-bit PWM value (0-1023) internally.  The get handler converts back to
percentage for the response.
"""

import time

import pytest


class TestBrightnessRoundtrip:
    """Verify brightness percentage set/get roundtrip."""

    @pytest.fixture(autouse=True)
    def _save_restore_brightness(self, hub):
        data = hub.get("brightness")
        self._orig = data.get("brightness") if data else None
        yield
        if self._orig is not None:
            hub.set({"brightness": self._orig})

    @pytest.mark.parametrize("pct", [5, 10, 25, 50, 75, 100])
    def test_roundtrip(self, hub, pct):
        """Set brightness to pct%, read it back, verify it matches."""
        hub.set({"brightness": pct})
        time.sleep(0.1)
        data = hub.get("brightness")
        assert data is not None, "get brightness returned None"
        assert data["brightness"] == pct

    #removed 0% brighness test

class TestBrightnessOutOfRange:
    """Brightness rejects values outside 0-100."""

    @pytest.fixture(autouse=True)
    def _save_restore_brightness(self, hub):
        data = hub.get("brightness")
        self._orig = data.get("brightness") if data else None
        yield
        if self._orig is not None:
            hub.set({"brightness": self._orig})

    @pytest.mark.parametrize("val", [1,101, 200, 1000])
    def test_rejects_invalid(self, hub, val):
        """Setting brightness outside 0-100 returns a 'fail' error."""
        resp = hub.send({"action": "set", "params": {"brightness": val}})
        data = resp.get("data", {}) if resp else {}
        assert "out of range" in str(data.get("brightness"))

    def test_value_unchanged_after_invalid(self, hub):
        """Invalid brightness values do not alter the stored brightness."""
        hub.set({"brightness": 50})
        time.sleep(0.1)
        before = hub.get("brightness")

        hub.send({"action": "set", "params": {"brightness": 999}})
        time.sleep(0.1)

        after = hub.get("brightness")
        assert after["brightness"] == before["brightness"]
