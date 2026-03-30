#!/usr/bin/env python3
"""Plasma demo — swirling color animation on the hub displays.

Usage:
    python demo_plasma.py                          # default: channel 2, ocean
    python demo_plasma.py --palette neon           # neon palette
    python demo_plasma.py --palette fire --fps     # fire palette with FPS stats
    python demo_plasma.py --channels 1,2,3         # all channels

Palettes: rainbow, neon, fire, ocean, lava

Ctrl-C to stop.
"""

import argparse
import json
import math
import sys
import time

import serial

from binary_transport import (
    IMG_FLAG_BUFFER, IMG_FLAG_DIRECT, IMG_FLAG_SPRITE,
    build_image_frame, rgb565_bytes,
)
from hub import find_hub

MODE_MAP = {"buffer": IMG_FLAG_BUFFER, "sprite": IMG_FLAG_SPRITE, "direct": IMG_FLAG_DIRECT}

WIDTH = 226
HEIGHT = 90


def hsv_to_rgb(h, s, v):
    """Convert HSV (0-1 range) to RGB (0-255)."""
    if s == 0:
        r = g = b = int(v * 255)
        return r, g, b
    i = int(h * 6.0)
    f = h * 6.0 - i
    p = int(v * (1 - s) * 255)
    q = int(v * (1 - s * f) * 255)
    t = int(v * (1 - s * (1 - f)) * 255)
    iv = int(v * 255)
    i %= 6
    if i == 0: return iv, t, p
    if i == 1: return q, iv, p
    if i == 2: return p, iv, t
    if i == 3: return p, q, iv
    if i == 4: return t, p, iv
    return iv, p, q


def _lerp_color(c1, c2, t):
    """Linearly interpolate between two (r,g,b) tuples."""
    return tuple(int(a + (b - a) * t) for a, b in zip(c1, c2))


def _gradient_lut(stops):
    """Build a 256-entry RGB565 LUT from a list of (position, (r,g,b)) stops.

    Positions are 0.0–1.0.  Colors are interpolated linearly between stops.
    """
    lut = []
    for i in range(256):
        t = i / 255.0
        # Find surrounding stops
        for j in range(len(stops) - 1):
            p0, c0 = stops[j]
            p1, c1 = stops[j + 1]
            if t <= p1 or j == len(stops) - 2:
                frac = (t - p0) / (p1 - p0) if p1 != p0 else 0.0
                r, g, b = _lerp_color(c0, c1, max(0.0, min(1.0, frac)))
                lut.append(rgb565_bytes(r, g, b))
                break
    return lut


PALETTES = {}


def _register(name, builder):
    PALETTES[name] = builder


# --- Palette definitions ---

def _palette_rainbow():
    """Full HSV hue wheel, max saturation and brightness."""
    lut = []
    for i in range(256):
        r, g, b = hsv_to_rgb(i / 256.0, 1.0, 1.0)
        lut.append(rgb565_bytes(r, g, b))
    return lut

_register("rainbow", _palette_rainbow)


def _palette_neon():
    """Hot neon: magenta → cyan → green → yellow → magenta."""
    return _gradient_lut([
        (0.00, (255, 0, 200)),   # hot pink
        (0.25, (0, 255, 255)),   # cyan
        (0.50, (0, 255, 50)),    # neon green
        (0.75, (255, 255, 0)),   # yellow
        (1.00, (255, 0, 200)),   # wrap back to pink
    ])

_register("neon", _palette_neon)


def _palette_fire():
    """Dark red → bright red → orange → yellow → white."""
    return _gradient_lut([
        (0.00, (40, 0, 0)),
        (0.25, (255, 0, 0)),
        (0.50, (255, 160, 0)),
        (0.75, (255, 255, 0)),
        (1.00, (255, 255, 200)),
    ])

_register("fire", _palette_fire)


def _palette_ocean():
    """Blue → turquoise → cyan → bright turquoise."""
    return _gradient_lut([
        (0.00, (0, 40, 255)),
        (0.30, (0, 150, 255)),
        (0.55, (0, 220, 240)),
        (0.80, (0, 255, 200)),
        (1.00, (0, 255, 255)),
    ])

_register("ocean", _palette_ocean)


def _palette_lava():
    """Black → deep red → orange (no bright highlights)."""
    return _gradient_lut([
        (0.00, (10, 0, 0)),
        (0.30, (150, 0, 0)),
        (0.60, (220, 40, 0)),
        (0.85, (255, 120, 0)),
        (1.00, (255, 160, 0)),
    ])

_register("lava", _palette_lava)


def build_plasma_lut(name="ocean"):
    """Build a 256-entry RGB565 palette by name."""
    builder = PALETTES.get(name)
    if not builder:
        available = ", ".join(sorted(PALETTES))
        raise ValueError(f"Unknown palette '{name}'. Available: {available}")
    return builder()


def render_plasma(t, palette, channel_offset=0.0):
    """Render a plasma frame. Returns raw RGB565 bytes.

    Uses 4 overlapping sine waves with different frequencies and
    phase offsets to create the classic plasma effect.
    """
    buf = bytearray(WIDTH * HEIGHT * 2)
    off = 0
    t = t * 0.5  # slow down animation
    # Pre-compute some time-varying terms
    ct1 = math.cos(t * 0.7 + channel_offset)
    st1 = math.sin(t * 0.5 + channel_offset)
    ct2 = math.cos(t * 0.3)
    st2 = math.sin(t * 0.9)

    for y in range(HEIGHT):
        yf = y / HEIGHT
        # Vertical sine component (shared across row)
        v1_base = math.sin(yf * 4.0 + t * 1.1 + channel_offset)
        v3_base = math.sin((yf * 3.0 + ct2) * 2.0)

        for x in range(WIDTH):
            xf = x / WIDTH

            # Layer 1: horizontal + vertical waves
            v1 = math.sin(xf * 6.0 + t * 0.8) + v1_base

            # Layer 2: radial wave from a moving center
            cx = 0.5 + 0.3 * ct1
            cy = 0.5 + 0.3 * st1
            dx = xf - cx
            dy = yf - cy
            dist = math.sqrt(dx * dx + dy * dy)
            v2 = math.sin(dist * 12.0 - t * 2.0 + channel_offset)

            # Layer 3: diagonal wave
            v3 = math.sin((xf * 5.0 + st2) * 2.0) + v3_base

            # Layer 4: another radial from opposite corner
            dx2 = xf - 0.5 - 0.2 * st2
            dy2 = yf - 0.5 + 0.2 * ct1
            v4 = math.sin(math.sqrt(dx2 * dx2 + dy2 * dy2) * 8.0 + t * 1.5)

            # Combine — use sin() to wrap the sum smoothly across the
            # full palette instead of averaging (which clusters near center)
            val = math.sin((v1 + v2 + v3 + v4) * 0.8)
            idx = int((val + 1.0) * 127.5)
            idx = max(0, min(255, idx))

            buf[off:off + 2] = palette[idx]
            off += 2

    return bytes(buf)


def main():
    parser = argparse.ArgumentParser(description="Plasma demo")
    parser.add_argument("--port", help="Serial port (auto-detect if omitted)")
    parser.add_argument(
        "--channels", type=str, default="2",
        help="Comma-separated channel list (default: 2)",
    )
    parser.add_argument(
        "--palette", type=str, default="ocean",
        choices=sorted(PALETTES),
        help="Color palette (default: rainbow)",
    )
    parser.add_argument("--fps", action="store_true", help="Print FPS stats")
    parser.add_argument(
        "--mode", choices=sorted(MODE_MAP), default="buffer",
        help="Image write mode (default: buffer)",
    )
    args = parser.parse_args()

    channels = [int(c) for c in args.channels.split(",")]

    port = args.port or find_hub()[0]
    if not port:
        print("No Insight Hub found. Pass --port or connect a hub.")
        sys.exit(1)

    print(f"Connecting to {port}...")
    ser = serial.Serial(port, 115200, timeout=2.0)
    time.sleep(0.5)
    ser.reset_input_buffer()

    # Verify hub is responsive
    ser.write(b'{"action":"get","params":["hubMode"]}\n')
    ser.flush()
    line = ser.readline().decode("utf-8", errors="replace").strip()
    if not line:
        print("Hub not responding.")
        sys.exit(1)
    print(f"Hub: {line}")

    # Lock screens we'll be writing to
    lock = {"screenLock": {f"CH{ch}": 1 for ch in channels}}
    ser.write((json.dumps({"action": "set", "params": lock}) + "\n").encode())
    ser.flush()
    ser.readline()  # consume response

    img_flags = MODE_MAP[args.mode]
    bpp = 16
    if img_flags == IMG_FLAG_SPRITE:
        bpp = 8
        print("Sprite mode: using 8bpp RGB332")

    palette = build_plasma_lut(args.palette)
    phase_offsets = {1: 0.0, 2: 2.094, 3: 4.189}  # 120° apart

    print(f"Plasma [{args.palette}] on CH{','.join(str(c) for c in channels)} "
          f"mode={args.mode} bpp={bpp} — Ctrl-C to stop")
    frame_count = 0
    t_start = time.monotonic()
    errors = 0

    try:
        while True:
            t = time.monotonic() - t_start

            for ch in channels:
                pixels = render_plasma(t, palette, phase_offsets.get(ch, 0.0))
                if bpp == 8:
                    # Convert RGB565 to RGB332
                    pixels8 = bytearray(WIDTH * HEIGHT)
                    for pi in range(WIDTH * HEIGHT):
                        hi = pixels[pi * 2]
                        lo = pixels[pi * 2 + 1]
                        raw = (hi << 8) | lo
                        r5 = (raw >> 11) & 0x1F
                        g6 = (raw >> 5) & 0x3F
                        b5 = raw & 0x1F
                        pixels8[pi] = (r5 >> 2 << 5) | (g6 >> 3 << 2) | (b5 >> 3)
                    frame = build_image_frame(ch, 8, WIDTH, HEIGHT, bytes(pixels8), flags=img_flags)
                else:
                    frame = build_image_frame(ch, bpp, WIDTH, HEIGHT, pixels, flags=img_flags)

                ser.write(frame)
                ser.flush()

                # Read JSON response
                resp_line = ser.readline().decode("utf-8", errors="replace").strip()
                if resp_line:
                    try:
                        resp = json.loads(resp_line)
                        if resp.get("status") != "ok":
                            errors += 1
                            if errors <= 3:
                                print(f"  Frame error: {resp_line}")
                    except json.JSONDecodeError:
                        errors += 1

                frame_count += 1

            if args.fps and frame_count % (10 * len(channels)) == 0:
                elapsed = time.monotonic() - t_start
                fps = frame_count / elapsed
                print(f"\r  {frame_count} frames, {fps:.1f} fps "
                      f"({fps / len(channels):.1f} per channel), "
                      f"{errors} errors    ", end="", flush=True)

    except KeyboardInterrupt:
        elapsed = time.monotonic() - t_start
        fps = frame_count / elapsed if elapsed > 0 else 0
        print(f"\n\n  {frame_count} frames in {elapsed:.1f}s = {fps:.1f} fps "
              f"({fps / len(channels):.1f} per channel)")
        if errors:
            print(f"  {errors} errors")

    # Unlock screens
    unlock = {"screenLock": {f"CH{ch}": 0 for ch in channels}}
    ser.write((json.dumps({"action": "set", "params": unlock}) + "\n").encode())
    ser.flush()
    ser.readline()
    ser.close()
    print("  Done.")


if __name__ == "__main__":
    main()
