"""Test screen lock (check-in/check-out) and ready notification.

Usage:
    pytest test_screen_lock.py -v -s
"""

import json
import logging
import time

import pytest

from binary_transport import (
    build_screen_lock_frame,
    build_screen_ready_frame,
    build_image_frame,
    solid_image_rgb565,
)

log = logging.getLogger("test_screen_lock")

IMAGE_WIDTH = 226
IMAGE_HEIGHT = 90


def send_bin_and_read_json(hub, frame, timeout=5.0):
    """Send a binary frame and read the JSON response."""
    old_timeout = hub.ser.timeout
    hub.ser.timeout = timeout
    try:
        hub.ser.write(frame)
        hub.ser.flush()
        line = hub.ser.readline().decode("utf-8", errors="replace").strip()
        log.info("Response: %s", line)
        return json.loads(line) if line else None
    finally:
        hub.ser.timeout = old_timeout


@pytest.fixture(autouse=True)
def unlock_all_channels(hub):
    """Ensure all channels are unlocked before each test."""
    hub.set({"screenLock": {"CH1": 0, "CH2": 0, "CH3": 0}})
    time.sleep(0.05)


class TestScreenLockJSON:
    """Test screen lock via JSON API."""

    def test_lock_unlock(self, hub):
        """Lock CH1 via JSON SET, verify GET, unlock, verify."""
        # Lock CH1
        hub.set({"screenLock": {"CH1": 1}})
        time.sleep(0.1)

        data = hub.get("screenLock")
        assert data is not None
        assert data["screenLock"]["CH1"] == 1
        assert data["screenLock"]["CH2"] == 0
        assert data["screenLock"]["CH3"] == 0

        # Unlock CH1
        hub.set({"screenLock": {"CH1": 0}})
        time.sleep(0.1)

        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 0

    def test_lock_multi_channel(self, hub):
        """Lock CH1 and CH3, verify CH2 unaffected."""
        hub.set({"screenLock": {"CH1": 1, "CH3": 1}})
        time.sleep(0.1)

        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 1
        assert data["screenLock"]["CH2"] == 0
        assert data["screenLock"]["CH3"] == 1

        # Cleanup
        hub.set({"screenLock": {"CH1": 0, "CH3": 0}})

    def test_lock_refresh(self, hub):
        """Re-locking refreshes the timeout."""
        hub.set({"screenLock": {"CH1": 1}})
        time.sleep(0.1)

        # Re-lock (refresh)
        hub.set({"screenLock": {"CH1": 1}})
        time.sleep(0.1)

        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 1

        # Cleanup
        hub.set({"screenLock": {"CH1": 0}})

    def test_image_implicit_lock(self, hub):
        """Sending an image implicitly locks the channel."""
        from test_binary_image import send_image_and_read_response

        # Send image to CH1
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0, 255)
        frame = build_image_frame(1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels)
        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok"

        time.sleep(0.1)

        # Verify CH1 is now locked
        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 1

        # Cleanup
        hub.set({"screenLock": {"CH1": 0}})

    def test_lock_appears_in_state(self, hub):
        """screenLock appears in 'state' wildcard GET."""
        hub.set({"screenLock": {"CH2": 1}})
        time.sleep(0.1)

        data = hub.get("state")
        assert "screenLock" in data
        assert data["screenLock"]["CH2"] == 1

        # Cleanup
        hub.set({"screenLock": {"CH2": 0}})


class TestScreenLockBinary:
    """Test screen lock via binary protocol."""

    def test_lock_unlock_binary(self, hub):
        """Lock CH1 via binary, verify via JSON GET, unlock."""
        # Lock CH1 (mask=0x01, action=1)
        frame = build_screen_lock_frame(0x01, 1)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "ok"
        assert "locked" in resp["data"]["message"]

        time.sleep(0.1)
        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 1

        # Unlock CH1 (mask=0x01, action=0)
        frame = build_screen_lock_frame(0x01, 0)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "ok"
        assert "unlocked" in resp["data"]["message"]

        time.sleep(0.1)
        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 0

    def test_lock_multi_binary(self, hub):
        """Lock CH1+CH3 via binary (mask=0x05)."""
        frame = build_screen_lock_frame(0x05, 1)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "ok"

        time.sleep(0.1)
        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 1
        assert data["screenLock"]["CH2"] == 0
        assert data["screenLock"]["CH3"] == 1

        # Cleanup
        build_screen_lock_frame(0x07, 0)
        hub.set({"screenLock": {"CH1": 0, "CH2": 0, "CH3": 0}})

    def test_invalid_mask(self, hub):
        """Mask 0x00 should be rejected."""
        frame = build_screen_lock_frame(0x00, 1)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "error"

    def test_invalid_action(self, hub):
        """Action > 1 should be rejected."""
        frame = build_screen_lock_frame(0x01, 2)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "error"


class TestScreenReady:
    """Test screen ready (wait-for-slot) notification."""

    def test_ready_locked_channel(self, hub):
        """SCREEN_READY on a locked channel responds within ~200ms."""
        # Lock CH1
        hub.set({"screenLock": {"CH1": 1}})
        time.sleep(0.1)

        # Send screen ready request
        frame = build_screen_ready_frame(1)
        t0 = time.monotonic()
        resp = send_bin_and_read_json(hub, frame)
        elapsed = time.monotonic() - t0
        log.info("Screen ready response in %.1fms", elapsed * 1000)

        assert resp["status"] == "ok"
        assert "ready" in resp["data"]["message"]
        # Should respond within ~250ms (one render cycle max)
        assert elapsed < 0.5

        # Cleanup
        hub.set({"screenLock": {"CH1": 0}})

    def test_ready_unlocked_fails(self, hub):
        """SCREEN_READY on an unlocked channel returns error."""
        # Ensure unlocked
        hub.set({"screenLock": {"CH1": 0}})
        time.sleep(0.1)

        frame = build_screen_ready_frame(1)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "error"
        assert "not locked" in resp["data"]["message"]

    def test_ready_invalid_channel(self, hub):
        """SCREEN_READY with channel 0 or 4 should fail."""
        frame = build_screen_ready_frame(0)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "error"

        frame = build_screen_ready_frame(4)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "error"

    def test_ready_refreshes_lock(self, hub):
        """SCREEN_READY refreshes the lock timeout."""
        hub.set({"screenLock": {"CH1": 1}})
        time.sleep(0.1)

        # Send ready (which refreshes the timeout)
        frame = build_screen_ready_frame(1)
        resp = send_bin_and_read_json(hub, frame)
        assert resp["status"] == "ok"

        time.sleep(0.1)

        # Verify still locked
        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 1

        # Cleanup
        hub.set({"screenLock": {"CH1": 0}})


class TestScreenLockTimeout:
    """Test auto-release timeout (requires waiting ~10s)."""

    @pytest.mark.slow
    @pytest.mark.timeout(30)
    def test_timeout_releases_lock(self, hub):
        """Lock expires after SCREEN_LOCK_TIMEOUT_MS (10s)."""
        hub.set({"screenLock": {"CH1": 1}})
        time.sleep(0.1)

        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 1

        # Wait for timeout (10s + margin)
        log.info("Waiting 11s for lock timeout...")
        time.sleep(11)

        data = hub.get("screenLock")
        assert data["screenLock"]["CH1"] == 0, "Lock should have auto-released"
