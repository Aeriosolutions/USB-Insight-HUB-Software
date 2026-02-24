"""Integration tests for restart command, uptime reporting, and USB reboot toggle.

These tests exercise the serial restart command and verify the reboot_enabled
config toggle gates 1200-baud bootloader entry.  The restart tests cause the
hub to reboot, so they run after other test files (pytest-ordering or file
naming ensures this).

Requires a real hub connected via USB.
"""

import time

import pytest

from hub import find_hub


# Config fields that we can set back via the serial API.
_RESTORABLE_CONFIG = (
    "startUpmode", "hubMode", "filterType", "refreshRate",
    "rotation", "brightness", "reboot_enabled",
)


@pytest.fixture(autouse=True, scope="module")
def _save_restore_config(hub):
    """Save config before this module's tests, restore afterward."""
    saved = hub.get("config")
    yield

    # Restore — hub may be in bootloader, recover first
    try:
        resp = hub.send({"action": "get", "params": ["hubMode"]})
        if resp is None:
            hub.boot_from_bootloader()
    except Exception:
        try:
            hub.boot_from_bootloader()
        except Exception:
            return

    if saved:
        restore = {k: saved[k] for k in _RESTORABLE_CONFIG if k in saved}
        if restore:
            hub.set(restore)


# ── Uptime ──────────────────────────────────────────────────


class TestUptime:
    """Uptime is reported via the serial API."""

    def test_uptime_present(self, hub):
        """get 'uptime' returns a positive integer."""
        data = hub.get("uptime")
        assert data is not None
        assert isinstance(data["uptime"], int)
        assert data["uptime"] > 0

    def test_uptime_in_state(self, hub):
        """Uptime is included in the 'state' meta-query."""
        data = hub.get("state")
        assert "uptime" in data

    def test_uptime_in_all(self, hub):
        """Uptime is included in the 'all' meta-query."""
        data = hub.get("all")
        assert "uptime" in data

    def test_uptime_increases(self, hub):
        """Two successive reads show increasing uptime."""
        t1 = hub.get("uptime")["uptime"]
        time.sleep(1.1)
        t2 = hub.get("uptime")["uptime"]
        assert t2 > t1


# ── Reboot config via serial API ────────────────────────────


class TestRebootConfig:
    """reboot_enabled is readable and writable via serial API."""

    @pytest.fixture(autouse=True)
    def _set_disabled(self, hub):
        """Start each test with reboot disabled."""
        hub.set({"reboot_enabled": 0})
        yield
        hub.set({"reboot_enabled": 0})

    def test_reboot_enabled_in_config(self, hub):
        """reboot_enabled appears in the config response."""
        data = hub.get("config")
        assert "reboot_enabled" in data

    def test_set_and_get_disabled(self, hub):
        """reboot_enabled reads back as 0 after being set to 0.

        Note: this verifies the set/get roundtrip, not the factory default.
        A true default-value test would require either exposing resetToDefault
        over serial, or extending the tests to use the WebSocket API
        (resetToDefault is currently WebSocket-only).
        """
        data = hub.get("reboot_enabled")
        assert data["reboot_enabled"] == 0

    def test_roundtrip_enable(self, hub):
        """Set reboot_enabled=1, read back, verify."""
        hub.set({"reboot_enabled": 1})
        time.sleep(0.1)
        data = hub.get("reboot_enabled")
        assert data["reboot_enabled"] == 1

    def test_roundtrip_disable(self, hub):
        """Set reboot_enabled=1 then 0, verify disabled."""
        hub.set({"reboot_enabled": 1})
        time.sleep(0.1)
        hub.set({"reboot_enabled": 0})
        time.sleep(0.1)
        data = hub.get("reboot_enabled")
        assert data["reboot_enabled"] == 0

    def test_rejects_invalid(self, hub):
        """Values other than 0/1 are rejected."""
        resp = hub.send({"action": "set", "params": {"reboot_enabled": 2}})
        data = resp.get("data", {}) if resp else {}
        assert data.get("reboot_enabled") == "fail"


# ── Restart command ─────────────────────────────────────────


@pytest.mark.slow
class TestRestart:
    """Serial restart command reboots the hub."""

    def test_restart_resets_uptime(self, hub):
        """Graceful restart resets uptime to near-zero."""
        before = hub.get("uptime")["uptime"]
        assert before > 0

        hub.send({"action": "restart", "params": {}})
        hub.reconnect()

        after = hub.get("uptime")["uptime"]
        assert after < 15_000, f"uptime too high after restart: {after}ms"

    def test_restart_immediate(self, hub):
        """Immediate restart (ESP.restart) also resets uptime."""
        before = hub.get("uptime")["uptime"]
        assert before > 0

        hub.send({"action": "restart", "params": {"immediate": True}})
        hub.reconnect()

        after = hub.get("uptime")["uptime"]
        assert after < 15_000, f"uptime too high after restart: {after}ms"


# ── USB Reboot toggle (1200-baud bootloader) ────────────────


@pytest.mark.slow
class TestRebootToggle:
    """The reboot_enabled config gates 1200-baud bootloader entry."""

    @pytest.fixture(autouse=True)
    def _ensure_disabled(self, hub):
        """Ensure reboot is disabled before and after each test."""
        hub.set({"reboot_enabled": 0})
        yield
        # Best-effort restore — hub may be in bootloader with closed serial
        try:
            if hub.ser and hub.ser.is_open:
                resp = hub.send({"action": "get", "params": ["hubMode"]})
                if resp is not None:
                    hub.set({"reboot_enabled": 0})
                    return
            # Hub unresponsive or serial closed — try bootloader recovery
            hub.boot_from_bootloader()
            hub.set({"reboot_enabled": 0})
        except Exception:
            try:
                hub.boot_from_bootloader()
                hub.set({"reboot_enabled": 0})
            except Exception:
                pass

    def test_disabled_ignores_1200_touch(self, hub):
        """With reboot disabled, 1200-baud touch does not enter bootloader."""
        uptime_before = hub.get("uptime")["uptime"]

        hub.touch_1200()

        # Reconnect at normal baud — hub should still be running
        hub._connect()
        hub._verify_responsive()

        uptime_after = hub.get("uptime")["uptime"]
        assert uptime_after >= uptime_before, (
            "Hub rebooted despite reboot being disabled"
        )

    def test_enabled_enters_bootloader(self, hub):
        """With reboot enabled, 1200-baud touch triggers bootloader."""
        hub.set({"reboot_enabled": 1})
        time.sleep(0.5)

        # Show visual indicator on displays before entering bootloader
        for ch in ("CH1", "CH2", "CH3"):
            hub.set({ch: {"Dev1_name": "BOOT", "numDev": 1}})

        hub.touch_1200()
        time.sleep(2.0)

        # Hub should NOT be responding to serial commands now
        found = find_hub()
        if found:
            try:
                hub._connect()
                resp = hub.send({"action": "get", "params": ["hubMode"]})
                in_bootloader = resp is None
            except Exception:
                in_bootloader = True
        else:
            in_bootloader = True

        # Recover from bootloader
        hub.boot_from_bootloader()

        # Disable reboot for safety
        hub.set({"reboot_enabled": 0})

        assert in_bootloader, "Hub did not enter bootloader after 1200-baud touch"
