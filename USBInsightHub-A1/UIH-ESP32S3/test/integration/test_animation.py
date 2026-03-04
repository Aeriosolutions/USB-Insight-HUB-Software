"""Animation benchmark — measure achievable frame rate.

Sends a sequence of frames to one or more channels and reports timing.

Usage:
    pytest test_animation.py -v -s
    pytest test_animation.py -v -s -k "bouncing"
"""

import json
import logging
import math
import struct
import time

import pytest

from binary_transport import build_image_frame, rgb565, solid_image_rgb565

log = logging.getLogger("test_animation")


@pytest.fixture(autouse=True)
def unlock_all_channels(hub):
    """Ensure all channels are unlocked before each test."""
    hub.set({"screenLock": {"CH1": 0, "CH2": 0, "CH3": 0}})
    time.sleep(0.05)


WIDTH = 226
HEIGHT = 90
BYTES_PER_FRAME = WIDTH * HEIGHT * 2  # 16bpp RGB565


def send_frame(hub, frame, timeout=5.0):
    """Send frame, read JSON response, return (elapsed, ok, resp_dict)."""
    old = hub.ser.timeout
    hub.ser.timeout = timeout
    try:
        t0 = time.monotonic()
        hub.ser.write(frame)
        hub.ser.flush()
        line = hub.ser.readline().decode("utf-8", errors="replace").strip()
        elapsed = time.monotonic() - t0
        if not line:
            log.warning("Empty response after %.3fs", elapsed)
            return elapsed, False, {}
        resp = json.loads(line)
        ok = resp.get("status") == "ok"
        if not ok:
            log.warning("Frame error (%.3fs): %s", elapsed, line)
        return elapsed, ok, resp
    finally:
        hub.ser.timeout = old


# ---------------------------------------------------------------------------
# Frame generators — pre-build pixel data for speed
# ---------------------------------------------------------------------------

def make_solid_frames(n, width, height):
    """N frames cycling through hues."""
    frames = []
    for i in range(n):
        # HSV-like hue rotation: vary R/G/B in a simple triangle wave
        phase = i / n * 6.0
        if phase < 1:
            r, g, b = 255, int(255 * phase), 0
        elif phase < 2:
            r, g, b = int(255 * (2 - phase)), 255, 0
        elif phase < 3:
            r, g, b = 0, 255, int(255 * (phase - 2))
        elif phase < 4:
            r, g, b = 0, int(255 * (4 - phase)), 255
        elif phase < 5:
            r, g, b = int(255 * (phase - 4)), 0, 255
        else:
            r, g, b = 255, 0, int(255 * (6 - phase))
        pixels = solid_image_rgb565(width, height, r, g, b)
        frames.append(build_image_frame(1, 16, width, height, pixels))
    return frames


def make_scrolling_bars(n, width, height, bar_width=16):
    """N frames of vertical bars scrolling horizontally."""
    frames = []
    for i in range(n):
        offset = i * 4  # 4 pixels per frame
        buf = bytearray()
        for _y in range(height):
            for x in range(width):
                stripe = ((x + offset) // bar_width) % 3
                if stripe == 0:
                    buf += struct.pack("<H", rgb565(255, 0, 0))
                elif stripe == 1:
                    buf += struct.pack("<H", rgb565(0, 255, 0))
                else:
                    buf += struct.pack("<H", rgb565(0, 0, 255))
        frames.append(build_image_frame(1, 16, width, height, bytes(buf)))
    return frames


def make_bouncing_ball(n, width, height, radius=12):
    """N frames of a white ball bouncing on black background."""
    frames = []
    # Ball trajectory: simple bounce
    vx, vy = 3.7, 2.3
    cx, cy = float(width // 2), float(height // 2)

    for _i in range(n):
        cx += vx
        cy += vy
        if cx - radius < 0 or cx + radius >= width:
            vx = -vx
            cx += vx * 2
        if cy - radius < 0 or cy + radius >= height:
            vy = -vy
            cy += vy * 2

        # Render frame
        buf = bytearray()
        bg = struct.pack("<H", rgb565(0, 0, 32))
        r2 = radius * radius
        icx, icy = int(cx), int(cy)
        for y in range(height):
            dy = y - icy
            dy2 = dy * dy
            for x in range(width):
                dx = x - icx
                if dx * dx + dy2 <= r2:
                    buf += struct.pack("<H", rgb565(255, 255, 255))
                else:
                    buf += bg
        frames.append(build_image_frame(1, 16, width, height, bytes(buf)))
    return frames


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

class TestAnimation:
    """Animation frame rate benchmarks."""

    def test_solid_color_cycle(self, hub):
        """Cycle through solid hue frames — best-case throughput."""
        n = 10
        log.info("Pre-building %d solid-color frames...", n)
        frames = make_solid_frames(n, WIDTH, HEIGHT)

        log.info("Streaming %d frames to CH1 (%d bytes each)...", n, len(frames[0]))
        times = []
        profs = []
        for i, frame in enumerate(frames):
            elapsed, ok, resp = send_frame(hub, frame)
            times.append(elapsed)
            prof = resp.get("data", {}).get("prof")
            if prof:
                profs.append(prof)
            assert ok, f"Frame {i} failed"

        total = sum(times)
        fps = n / total
        avg = total / n
        log.info(
            "Solid cycle: %d frames in %.2fs — %.2f FPS (avg %.3fs/frame)",
            n, total, fps, avg,
        )

        # Print profiling breakdown
        if profs:
            log.info("--- Device-side profiling (micros) ---")
            log.info("%-6s %8s %8s %8s %8s %8s  %5s %6s %5s %5s",
                     "Frame", "total", "usb", "crc", "sem", "spi",
                     "reads", "bytes", "wakes", "avg_rd")
            for i, p in enumerate(profs):
                log.info("%-6d %8d %8d %8d %8d %8d  %5d %6d %5d %5d",
                         i, p["total_us"], p["usb_us"], p["crc_us"],
                         p["sem_us"], p["spi_us"],
                         p["reads"], p["bytes"], p["wakes"], p["avg_read"])

        assert n == len(times)

    def test_scrolling_bars(self, hub):
        """Scrolling colored bars — measures complex frame throughput."""
        n = 10
        log.info("Pre-building %d scrolling bar frames...", n)
        frames = make_scrolling_bars(n, WIDTH, HEIGHT)

        log.info("Streaming %d frames to CH1...", n)
        times = []
        for i, frame in enumerate(frames):
            elapsed, ok, _resp = send_frame(hub, frame)
            times.append(elapsed)
            assert ok, f"Frame {i} failed"

        total = sum(times)
        fps = n / total
        log.info(
            "Scrolling bars: %d frames in %.2fs — %.2f FPS (avg %.3fs/frame)",
            n, total, fps, total / n,
        )

    def test_bouncing_ball(self, hub):
        """Bouncing ball animation — realistic mixed-content frames."""
        n = 15
        log.info("Pre-building %d bouncing ball frames...", n)
        frames = make_bouncing_ball(n, WIDTH, HEIGHT)

        log.info("Streaming %d frames to CH1...", n)
        times = []
        for i, frame in enumerate(frames):
            elapsed, ok, _resp = send_frame(hub, frame)
            times.append(elapsed)
            assert ok, f"Frame {i} failed"

        total = sum(times)
        fps = n / total
        fastest = min(times)
        slowest = max(times)
        log.info(
            "Bouncing ball: %d frames in %.2fs — %.2f FPS "
            "(avg %.3fs, min %.3fs, max %.3fs)",
            n, total, fps, total / n, fastest, slowest,
        )

    def test_small_region_fps(self, hub):
        """Smaller image (64x64) to measure FPS with less data."""
        w, h = 64, 64
        n = 20
        log.info("Pre-building %d small frames (%dx%d)...", n, w, h)
        frames = []
        for i in range(n):
            phase = i / n
            r = int(128 + 127 * math.sin(phase * 2 * math.pi))
            g = int(128 + 127 * math.sin(phase * 2 * math.pi + 2.094))
            b = int(128 + 127 * math.sin(phase * 2 * math.pi + 4.189))
            pixels = solid_image_rgb565(w, h, r, g, b)
            frames.append(build_image_frame(1, 16, w, h, pixels))

        log.info(
            "Streaming %d frames (%d bytes each)...", n, len(frames[0])
        )
        times = []
        for i, frame in enumerate(frames):
            elapsed, ok, resp = send_frame(hub, frame)
            times.append(elapsed)
            prof = resp.get("data", {}).get("prof")
            if prof and i == 0:
                log.info(
                    "Small prof: total=%dus usb=%dus sem=%dus spi=%dus "
                    "reads=%d bytes=%d avg_rd=%d",
                    prof["total_us"], prof["usb_us"], prof["sem_us"],
                    prof["spi_us"], prof["reads"], prof["bytes"],
                    prof["avg_read"],
                )
            assert ok, f"Frame {i} failed"

        total = sum(times)
        fps = n / total
        log.info(
            "Small region: %d frames (%dx%d) in %.2fs — %.2f FPS "
            "(avg %.3fs/frame, %d bytes/frame)",
            n, w, h, total, fps, total / n, len(frames[0]),
        )

    def test_three_channel_round_robin(self, hub):
        """Send frames to CH1, CH2, CH3 in round-robin."""
        n_per_ch = 5
        log.info("Pre-building %d frames per channel...", n_per_ch)
        ch_frames = {}
        for ch in (1, 2, 3):
            ch_frames[ch] = []
            for i in range(n_per_ch):
                # Each channel gets a different base color
                colors = {1: (255, 0, 0), 2: (0, 255, 0), 3: (0, 0, 255)}
                r, g, b = colors[ch]
                bright = 0.3 + 0.7 * (i / n_per_ch)
                pixels = solid_image_rgb565(
                    WIDTH, HEIGHT,
                    int(r * bright), int(g * bright), int(b * bright),
                )
                ch_frames[ch].append(
                    build_image_frame(ch, 16, WIDTH, HEIGHT, pixels)
                )

        log.info("Round-robin across 3 channels...")
        times = []
        total_frames = 0
        for i in range(n_per_ch):
            for ch in (1, 2, 3):
                elapsed, ok, _resp = send_frame(hub, ch_frames[ch][i])
                times.append(elapsed)
                total_frames += 1
                assert ok, f"CH{ch} frame {i} failed"

        total = sum(times)
        fps = total_frames / total
        log.info(
            "Round-robin: %d total frames (3 ch × %d) in %.2fs — %.2f FPS "
            "(avg %.3fs/frame)",
            total_frames, n_per_ch, total, fps, total / total_frames,
        )
