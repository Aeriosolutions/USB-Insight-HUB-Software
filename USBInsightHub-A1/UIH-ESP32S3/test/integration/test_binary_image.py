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
    IMG_FLAG_BUFFER,
    IMG_FLAG_DIRECT,
    IMG_FLAG_RLE,
    IMG_FLAG_SPRITE,
    build_image_frame,
    gradient_image_rgb565,
    solid_image_rgb565,
)

log = logging.getLogger("test_binary_image")

IMAGE_WIDTH = 226
IMAGE_HEIGHT = 90


def send_image_and_read_response(hub, frame, timeout=10.0):
    """Send an image frame and read the JSON response with extended timeout.

    Includes a short drain to handle leftover data from previous frames
    (e.g. after a rejected frame where ~40KB payload still needs processing).
    """
    # Let the hub finish processing any in-flight data from prior frames
    time.sleep(0.15)
    hub.ser.reset_input_buffer()

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


def _solid_rgb332(width, height, color_byte):
    """Generate a solid-color 8bpp RGB332 image."""
    return bytes([color_byte]) * (width * height)


class TestImageModes:
    """Test the 3 image write modes: buffer, sprite, direct."""

    def test_buffer_mode_explicit(self, hub):
        """Buffer mode (flags=0) still works when set explicitly."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 255, 0, 0)
        frame = build_image_frame(
            1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels, flags=IMG_FLAG_BUFFER
        )
        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok"
        assert "image complete" in resp["data"]["message"]

    def test_sprite_mode_8bpp(self, hub):
        """Sprite mode (flags=1) with 8bpp should succeed."""
        pixels = _solid_rgb332(IMAGE_WIDTH, IMAGE_HEIGHT, 0xE0)  # red in RGB332
        frame = build_image_frame(
            1, 8, IMAGE_WIDTH, IMAGE_HEIGHT, pixels, flags=IMG_FLAG_SPRITE
        )
        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok"
        assert "sprite complete" in resp["data"]["message"]

    def test_sprite_mode_rejects_16bpp(self, hub):
        """Sprite mode with 16bpp should be rejected."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 0, 255, 0)
        frame = build_image_frame(
            1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels, flags=IMG_FLAG_SPRITE
        )
        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "error"
        assert "8bpp" in resp["data"]["message"]

    def test_direct_mode_16bpp(self, hub):
        """Direct mode (flags=2) with 16bpp should succeed."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0, 255)
        frame = build_image_frame(
            1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels, flags=IMG_FLAG_DIRECT
        )
        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok", f"Expected ok, got: {resp}"
        assert "direct complete" in resp["data"]["message"]

    def test_direct_mode_8bpp(self, hub):
        """Direct mode with 8bpp should also work."""
        pixels = _solid_rgb332(IMAGE_WIDTH, IMAGE_HEIGHT, 0x1C)  # green in RGB332
        frame = build_image_frame(
            2, 8, IMAGE_WIDTH, IMAGE_HEIGHT, pixels, flags=IMG_FLAG_DIRECT
        )
        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok"
        assert "direct complete" in resp["data"]["message"]

    def test_invalid_flags(self, hub):
        """Flags=3 (invalid) should be rejected."""
        pixels = _solid_rgb332(IMAGE_WIDTH, IMAGE_HEIGHT, 0xFF)
        frame = build_image_frame(
            1, 8, IMAGE_WIDTH, IMAGE_HEIGHT, pixels, flags=3
        )
        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "error"
        assert "invalid flags" in resp["data"]["message"]

    def test_modes_sequence(self, hub):
        """Send images in all 3 modes sequentially — all should succeed."""
        w, h = IMAGE_WIDTH, IMAGE_HEIGHT

        # Buffer mode (16bpp)
        frame = build_image_frame(
            1, 16, w, h, solid_image_rgb565(w, h, 255, 0, 0), flags=IMG_FLAG_BUFFER
        )
        resp = json.loads(send_image_and_read_response(hub, frame))
        assert resp["status"] == "ok"

        # Sprite mode (8bpp)
        frame = build_image_frame(
            2, 8, w, h, _solid_rgb332(w, h, 0xE0), flags=IMG_FLAG_SPRITE
        )
        resp = json.loads(send_image_and_read_response(hub, frame))
        assert resp["status"] == "ok"

        # Direct mode (16bpp)
        frame = build_image_frame(
            3, 16, w, h, solid_image_rgb565(w, h, 0, 0, 255), flags=IMG_FLAG_DIRECT
        )
        resp = json.loads(send_image_and_read_response(hub, frame))
        assert resp["status"] == "ok"

        # Back to buffer mode
        frame = build_image_frame(
            1, 16, w, h, solid_image_rgb565(w, h, 0, 255, 0), flags=IMG_FLAG_BUFFER
        )
        resp = json.loads(send_image_and_read_response(hub, frame))
        assert resp["status"] == "ok"

    def test_timeout_recovery(self, hub):
        """Send a truncated frame, wait for timeout, then send a good frame."""
        w, h = IMAGE_WIDTH, IMAGE_HEIGHT
        pixels = solid_image_rgb565(w, h, 255, 255, 0)
        frame = build_image_frame(1, 16, w, h, pixels)

        # Send only the first half of the frame (truncated)
        half = len(frame) // 2
        hub.ser.write(frame[:half])
        hub.ser.flush()

        # Wait for the binary parse timeout (2s) + margin
        log.info("Waiting for timeout recovery (3s)...")
        time.sleep(3.0)

        # Drain any timeout error response
        hub.ser.reset_input_buffer()

        # Now send a complete, valid frame — should succeed
        frame2 = build_image_frame(1, 16, w, h, solid_image_rgb565(w, h, 0, 255, 0))
        line = send_image_and_read_response(hub, frame2)
        resp = json.loads(line)
        assert resp["status"] == "ok"


class TestRLECompression:
    """Test RLE-compressed image transfers."""

    def test_rle_buffer_mode(self, hub):
        """RLE + buffer mode (16bpp) — solid red should compress well."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 255, 0, 0)
        frame = build_image_frame(1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels,
                                  compress=True)
        log.info("Sending RLE buffer solid red (%d bytes)", len(frame))

        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok"
        assert "image complete" in resp["data"]["message"]

    def test_rle_sprite_mode(self, hub):
        """RLE + sprite mode (8bpp) — solid color."""
        pixels = _solid_rgb332(IMAGE_WIDTH, IMAGE_HEIGHT, 0xE0)
        frame = build_image_frame(1, 8, IMAGE_WIDTH, IMAGE_HEIGHT, pixels,
                                  flags=IMG_FLAG_SPRITE, compress=True)
        log.info("Sending RLE sprite solid red (%d bytes)", len(frame))

        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok"
        assert "sprite complete" in resp["data"]["message"]

    def test_rle_direct_mode(self, hub):
        """RLE + direct mode (16bpp) — solid blue."""
        pixels = solid_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0, 255)
        frame = build_image_frame(1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels,
                                  flags=IMG_FLAG_DIRECT, compress=True)
        log.info("Sending RLE direct solid blue (%d bytes)", len(frame))

        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok"
        assert "direct complete" in resp["data"]["message"]

    def test_rle_gradient(self, hub):
        """RLE with gradient (poor compression) should still work."""
        pixels = gradient_image_rgb565(IMAGE_WIDTH, IMAGE_HEIGHT)
        frame = build_image_frame(1, 16, IMAGE_WIDTH, IMAGE_HEIGHT, pixels,
                                  compress=True)
        log.info("Sending RLE gradient (%d bytes)", len(frame))

        line = send_image_and_read_response(hub, frame)
        resp = json.loads(line)
        assert resp["status"] == "ok"

    def test_json_after_rle(self, hub):
        """JSON API still works after RLE image transfer."""
        pixels = _solid_rgb332(IMAGE_WIDTH, IMAGE_HEIGHT, 0x1C)
        frame = build_image_frame(2, 8, IMAGE_WIDTH, IMAGE_HEIGHT, pixels,
                                  flags=IMG_FLAG_SPRITE, compress=True)
        send_image_and_read_response(hub, frame)

        time.sleep(0.5)
        hub.ser.reset_input_buffer()
        data = hub.get("hubMode")
        assert data is not None
        assert "hubMode" in data
