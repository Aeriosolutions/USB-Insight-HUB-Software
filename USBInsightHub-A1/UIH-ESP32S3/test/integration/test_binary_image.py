"""Test binary transport image command.

Sends test patterns to each display channel via the binary protocol
and verifies the JSON response.

Usage:
    pytest test_binary_image.py -v -s
"""

import json
import logging
import time

import pytest

from binary_transport import (
    build_image_frame,
    gradient_image_rgb565,
    solid_image_rgb565,
)

log = logging.getLogger("test_binary_image")

IMAGE_WIDTH = 226
IMAGE_HEIGHT = 90


def send_image_and_read_response(hub, frame, timeout=10.0):
    """Send an image frame and read the JSON response with extended timeout."""
    old_timeout = hub.ser.timeout
    hub.ser.timeout = timeout
    try:
        t0 = time.monotonic()
        hub.ser.write(frame)
        hub.ser.flush()
        line = hub.ser.readline().decode("utf-8", errors="replace").strip()
        elapsed = time.monotonic() - t0
        log.info("Response (%.1fs): %s", elapsed, line)
        return line
    finally:
        hub.ser.timeout = old_timeout


class TestBinaryImage:
    """Test direct-to-screen image streaming via binary transport."""

    def test_solid_red_ch1(self, hub):
        """Send a solid red image to channel 1."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 255, 0, 0)
        frame = build_image_frame(1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels)
        log.info("Sending solid red to CH1 (%d bytes)", len(frame))

        line = send_image_and_read_response(hub, frame)

        resp = json.loads(line)
        assert resp["status"] == "ok"
        assert resp["data"]["cmd"] == 1
        assert "image complete" in resp["data"]["message"]

    def test_solid_green_ch2(self, hub):
        """Send a solid green image to channel 2."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 0, 255, 0)
        frame = build_image_frame(2, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels)
        log.info("Sending solid green to CH2 (%d bytes)", len(frame))

        line = send_image_and_read_response(hub, frame)

        resp = json.loads(line)
        assert resp["status"] == "ok"

    def test_gradient_ch3(self, hub):
        """Send a gradient image to channel 3."""
        pixels = gradient_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT)
        frame = build_image_frame(3, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels)
        log.info("Sending gradient to CH3 (%d bytes)", len(frame))

        line = send_image_and_read_response(hub, frame)

        resp = json.loads(line)
        assert resp["status"] == "ok"

    def test_json_still_works_after_image(self, hub):
        """Verify JSON API is unaffected after binary image transfer."""
        # Send an image first
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0, 255)
        frame = build_image_frame(1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels)
        line = send_image_and_read_response(hub, frame)

        time.sleep(0.1)

        # Now send a normal JSON command
        data = hub.get("hubMode")
        assert data is not None
        assert "hubMode" in data

    def test_invalid_port(self, hub):
        """Port 0 should be rejected."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 255, 255, 255)
        frame = build_image_frame(0, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels)

        line = send_image_and_read_response(hub, frame)

        resp = json.loads(line)
        assert resp["status"] == "error"

    def test_bad_checksum(self, hub):
        """Corrupted frame should be rejected."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 128, 128, 128)
        frame = bytearray(
            build_image_frame(1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels)
        )
        # Corrupt the last byte (checksum)
        frame[-1] ^= 0xFF

        line = send_image_and_read_response(hub, bytes(frame))

        resp = json.loads(line)
        assert resp["status"] == "error"
        assert "checksum" in resp["data"]["message"]
