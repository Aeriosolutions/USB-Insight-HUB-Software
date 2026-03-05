#!/usr/bin/env python3
"""Tear test — high-contrast horizontal bars scrolling vertically.

Designed to make image tearing immediately visible. If the display
updates mid-frame, you'll see a horizontal offset where the sharp
black/white boundary is displaced.

Usage:
    python demo_tear_test.py [--port /dev/cu.usbmodem...]
    python demo_tear_test.py --bar-height 8          # thinner bars
    python demo_tear_test.py --speed 4               # pixels per frame
    python demo_tear_test.py --mode direct           # test direct SPI mode

Ctrl-C to stop.
"""

import argparse
import json
import struct
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


def render_hbars(offset, bar_height, palette_a, palette_b):
    """Render horizontal bars scrolling downward.

    Returns raw RGB565 bytes. The bars are full-width so any
    horizontal discontinuity (tear) is immediately visible.
    """
    buf = bytearray(WIDTH * HEIGHT * 2)
    off = 0
    for y in range(HEIGHT):
        stripe = ((y + offset) // bar_height) % 2
        pixel = palette_a if stripe == 0 else palette_b
        for _x in range(WIDTH):
            buf[off:off + 2] = pixel
            off += 2
    return bytes(buf)


def render_checker(offset, cell_size, palette_a, palette_b):
    """Render a checkerboard scrolling diagonally.

    Diagonal motion makes tears visible in both axes.
    """
    buf = bytearray(WIDTH * HEIGHT * 2)
    off = 0
    for y in range(HEIGHT):
        for x in range(WIDTH):
            cx = ((x + offset) // cell_size) % 2
            cy = ((y + offset) // cell_size) % 2
            pixel = palette_a if (cx ^ cy) else palette_b
            buf[off:off + 2] = pixel
            off += 2
    return bytes(buf)


def main():
    parser = argparse.ArgumentParser(description="Tear test")
    parser.add_argument("--port", help="Serial port (auto-detect if omitted)")
    parser.add_argument("--channel", type=int, default=1, choices=[1, 2, 3])
    parser.add_argument(
        "--bar-height", type=int, default=16,
        help="Bar height in pixels (default: 16)",
    )
    parser.add_argument(
        "--speed", type=int, default=2,
        help="Scroll speed in pixels per frame (default: 2)",
    )
    parser.add_argument(
        "--pattern", choices=["bars", "checker", "both"], default="both",
        help="Pattern type (default: both — bars on CH1, checker on CH2)",
    )
    parser.add_argument(
        "--mode", choices=sorted(MODE_MAP), default="buffer",
        help="Image write mode (default: buffer)",
    )
    args = parser.parse_args()

    port = args.port or find_hub()
    if not port:
        print("No Insight Hub found. Pass --port or connect a hub.")
        sys.exit(1)

    print(f"Connecting to {port}...")
    ser = serial.Serial(port, 115200, timeout=2.0)
    time.sleep(0.5)
    ser.reset_input_buffer()

    # Verify
    ser.write(b'{"action":"get","params":["hubMode"]}\n')
    ser.flush()
    line = ser.readline().decode("utf-8", errors="replace").strip()
    if not line:
        print("Hub not responding.")
        sys.exit(1)
    print(f"Hub: {line}")

    img_flags = MODE_MAP[args.mode]
    bpp = 16
    if img_flags == IMG_FLAG_SPRITE:
        bpp = 8
        print("Sprite mode: using 8bpp RGB332")

    # Determine which channels to use
    if args.pattern == "bars":
        channels = [args.channel]
    elif args.pattern == "checker":
        channels = [args.channel]
    else:  # both
        channels = [1, 2]

    # Lock screens
    lock = {"screenLock": {f"CH{ch}": 1 for ch in channels}}
    ser.write((json.dumps({"action": "set", "params": lock}) + "\n").encode())
    ser.flush()
    ser.readline()

    white = rgb565_bytes(255, 255, 255)
    black = rgb565_bytes(0, 0, 0)
    red = rgb565_bytes(255, 0, 0)
    blue = rgb565_bytes(0, 0, 255)

    print(f"Tear test on CH{','.join(str(c) for c in channels)} "
          f"mode={args.mode} bpp={bpp} "
          f"(bar={args.bar_height}px, speed={args.speed}px/frame) — Ctrl-C to stop")
    print("Look for horizontal discontinuities in the bar/checker pattern.")

    frame_count = 0
    offset = 0
    t_start = time.monotonic()

    try:
        while True:
            for ch in channels:
                if args.pattern == "checker" or (args.pattern == "both" and ch == 2):
                    pixels = render_checker(
                        offset, args.bar_height, red, blue
                    )
                else:
                    pixels = render_hbars(
                        offset, args.bar_height, white, black
                    )

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
                    frame_data = build_image_frame(ch, 8, WIDTH, HEIGHT, bytes(pixels8), flags=img_flags)
                else:
                    frame_data = build_image_frame(ch, bpp, WIDTH, HEIGHT, pixels, flags=img_flags)
                ser.write(frame_data)
                ser.flush()

                resp = ser.readline().decode("utf-8", errors="replace").strip()
                if resp:
                    try:
                        r = json.loads(resp)
                        if r.get("status") != "ok":
                            print(f"  Error: {resp}")
                    except json.JSONDecodeError:
                        pass

                frame_count += 1

            offset += args.speed

            if frame_count % (20 * len(channels)) == 0:
                elapsed = time.monotonic() - t_start
                fps = frame_count / elapsed
                print(f"\r  {frame_count} frames, {fps:.1f} fps "
                      f"({fps / len(channels):.1f} per ch)    ",
                      end="", flush=True)

    except KeyboardInterrupt:
        elapsed = time.monotonic() - t_start
        fps = frame_count / elapsed if elapsed > 0 else 0
        print(f"\n\n  {frame_count} frames in {elapsed:.1f}s = {fps:.1f} fps")

    # Unlock
    unlock = {"screenLock": {f"CH{ch}": 0 for ch in channels}}
    ser.write((json.dumps({"action": "set", "params": unlock}) + "\n").encode())
    ser.flush()
    ser.readline()
    ser.close()
    print("  Done.")


if __name__ == "__main__":
    main()
