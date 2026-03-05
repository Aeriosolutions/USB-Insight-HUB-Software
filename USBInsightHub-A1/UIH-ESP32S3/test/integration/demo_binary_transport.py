#!/usr/bin/env python3
"""Demo for the binary transport protocol.

Showcases image streaming and echo commands via the binary frame protocol.
Sends test patterns to each display and measures round-trip echo latency.

Usage:
    python demo_binary_transport.py [--port /dev/cu.usbmodem...]

Requires: pyserial
"""

import argparse
import json
import math
import struct
import sys
import time

import serial

from binary_transport import (
    BIN_CMD_ECHO,
    build_echo_frame,
    build_image_frame,
    crc32,
    parse_frame,
    rgb565_bytes,
    solid_image_rgb565,
)
from hub import Hub, find_hub

# Display geometry (image area within the 240x240 ST7789)
IMG_WIDTH = 226
IMG_HEIGHT = 90


def gradient_image(width, height):
    """Red-to-blue horizontal gradient."""
    pixels = bytearray()
    for y in range(height):
        for x in range(width):
            r = int(255 * (1 - x / width))
            b = int(255 * x / width)
            g = int(128 * y / height)
            pixels += rgb565_bytes(r, g, b)
    return bytes(pixels)


def rainbow_bars(width, height):
    """Vertical rainbow bars — easy visual check that colors are correct."""
    colors = [
        (255, 0, 0),      # red
        (255, 128, 0),    # orange
        (255, 255, 0),    # yellow
        (0, 255, 0),      # green
        (0, 255, 255),    # cyan
        (0, 0, 255),      # blue
        (128, 0, 255),    # violet
        (255, 0, 255),    # magenta
    ]
    bar_width = width // len(colors)
    pixels = bytearray()
    for _y in range(height):
        for x in range(width):
            idx = min(x // bar_width, len(colors) - 1)
            r, g, b = colors[idx]
            pixels += rgb565_bytes(r, g, b)
    return bytes(pixels)


def checkerboard(width, height, block=8):
    """Black-and-white checkerboard — tests pixel alignment."""
    pixels = bytearray()
    for y in range(height):
        for x in range(width):
            white = ((x // block) + (y // block)) % 2 == 0
            if white:
                pixels += rgb565_bytes(255, 255, 255)
            else:
                pixels += rgb565_bytes(0, 0, 0)
    return bytes(pixels)


def plasma(width, height, t=0.0):
    """Plasma effect — animated if called with varying t."""
    pixels = bytearray()
    for y in range(height):
        for x in range(width):
            # Classic plasma formula
            v1 = math.sin(x / 16.0 + t)
            v2 = math.sin(y / 8.0 + t * 0.7)
            v3 = math.sin((x + y) / 16.0 + t * 1.3)
            v4 = math.sin(math.sqrt(x * x + y * y) / 8.0 + t * 0.5)
            v = (v1 + v2 + v3 + v4) / 4.0  # -1..1
            # Map to RGB
            r = int(128 + 127 * math.sin(v * math.pi))
            g = int(128 + 127 * math.sin(v * math.pi + 2.094))
            b = int(128 + 127 * math.sin(v * math.pi + 4.189))
            pixels += rgb565_bytes(r, g, b)
    return bytes(pixels)


def send_image(ser, port, pixels, label=""):
    """Send an image frame and print the response."""
    frame = build_image_frame(port, 16, IMG_WIDTH, IMG_HEIGHT, pixels)
    t0 = time.monotonic()
    ser.write(frame)
    ser.flush()
    line = ser.readline().decode("utf-8", errors="replace").strip()
    elapsed = (time.monotonic() - t0) * 1000

    if line:
        try:
            resp = json.loads(line)
            status = resp.get("status", "?")
            msg = resp.get("data", {}).get("message", "")
            tag = "OK" if status == "ok" else f"ERR: {msg}"
        except json.JSONDecodeError:
            tag = f"bad response: {line[:60]}"
    else:
        tag = "no response (timeout)"

    desc = f"CH{port}"
    if label:
        desc += f" {label}"
    print(f"  {desc}: {len(frame):,} bytes, {elapsed:.0f}ms — {tag}")
    return tag.startswith("OK")


def echo_test(ser, payload, label=""):
    """Send an echo frame and verify the round-trip."""
    frame = build_echo_frame(payload)
    t0 = time.monotonic()
    ser.write(frame)
    ser.flush()

    # Read binary response
    expected_size = 1 + 9 + len(payload) + 4  # \0 + header + payload + crc32
    response = b""
    deadline = time.monotonic() + 3.0
    while len(response) < expected_size and time.monotonic() < deadline:
        chunk = ser.read(expected_size - len(response))
        if chunk:
            response += chunk
        else:
            time.sleep(0.005)
    elapsed = (time.monotonic() - t0) * 1000

    desc = label or f"{len(payload)}B echo"
    if len(response) < expected_size:
        print(f"  {desc}: TIMEOUT ({len(response)}/{expected_size} bytes, {elapsed:.0f}ms)")
        return False

    try:
        cmd, flags, echoed = parse_frame(response[:expected_size])
        if cmd != BIN_CMD_ECHO:
            print(f"  {desc}: wrong cmd {cmd}")
            return False
        if echoed != payload:
            print(f"  {desc}: payload mismatch!")
            return False
        print(f"  {desc}: OK, {elapsed:.1f}ms round-trip")
        return True
    except ValueError as e:
        print(f"  {desc}: parse error: {e}")
        return False


def main():
    parser = argparse.ArgumentParser(description="Binary transport demo")
    parser.add_argument("--port", help="Serial port (auto-detect if omitted)")
    parser.add_argument("--skip-images", action="store_true",
                        help="Skip image tests (echo only)")
    parser.add_argument("--animate", action="store_true",
                        help="Run animated plasma on CH1 (Ctrl-C to stop)")
    args = parser.parse_args()

    port = args.port or find_hub()[0]
    if not port:
        print("No Insight Hub found. Pass --port or connect a hub.")
        sys.exit(1)

    print(f"Connecting to {port}...")
    ser = serial.Serial(port, 115200, timeout=2.0)
    time.sleep(0.5)
    ser.reset_input_buffer()

    # Quick JSON check
    ser.write(b'{"action":"get","params":["esp32_ver"]}\n')
    ser.flush()
    line = ser.readline().decode("utf-8", errors="replace").strip()
    if line:
        try:
            resp = json.loads(line)
            ver = resp.get("data", {}).get("cpu_ver", "?")
            print(f"Hub firmware: {ver}")
        except json.JSONDecodeError:
            print(f"Unexpected response: {line[:80]}")
    else:
        print("Warning: hub not responding to JSON")

    print()

    # --- Echo tests ---
    print("Echo tests:")
    echo_test(ser, b"Hello, Insight Hub!", "greeting")
    echo_test(ser, b"", "empty")
    echo_test(ser, bytes(range(256)), "256B binary")
    echo_test(ser, b"A" * 1024, "1KB")
    echo_test(ser, b"X" * 4096, "4KB (max)")

    # Verify JSON still works after binary
    print()
    print("JSON after binary:")
    ser.write(b'{"action":"get","params":["hubMode"]}\n')
    ser.flush()
    line = ser.readline().decode("utf-8", errors="replace").strip()
    if line:
        try:
            resp = json.loads(line)
            print(f"  hubMode: {resp.get('data', {}).get('hubMode', '?')} — OK")
        except json.JSONDecodeError:
            print(f"  bad response: {line[:60]}")
    else:
        print("  no response (timeout)")

    if args.skip_images:
        ser.close()
        return

    print()

    # --- Image tests ---
    print("Image tests:")
    print(f"  Display area: {IMG_WIDTH}x{IMG_HEIGHT} RGB565 "
          f"({IMG_WIDTH * IMG_HEIGHT * 2:,} bytes/image)")
    print()

    print("Solid fills:")
    send_image(ser, 1, solid_image_rgb565(IMG_WIDTH, IMG_HEIGHT, 255, 0, 0), "red")
    send_image(ser, 2, solid_image_rgb565(IMG_WIDTH, IMG_HEIGHT, 0, 255, 0), "green")
    send_image(ser, 3, solid_image_rgb565(IMG_WIDTH, IMG_HEIGHT, 0, 0, 255), "blue")
    time.sleep(1.0)

    print()
    print("Patterns:")
    send_image(ser, 1, rainbow_bars(IMG_WIDTH, IMG_HEIGHT), "rainbow")
    send_image(ser, 2, gradient_image(IMG_WIDTH, IMG_HEIGHT), "gradient")
    send_image(ser, 3, checkerboard(IMG_WIDTH, IMG_HEIGHT), "checkerboard")
    time.sleep(1.0)

    print()
    print("Plasma:")
    send_image(ser, 1, plasma(IMG_WIDTH, IMG_HEIGHT, 0.0), "static")

    if args.animate:
        print()
        print("Animating plasma on CH1 (Ctrl-C to stop)...")
        frame_count = 0
        t_start = time.monotonic()
        try:
            t = 0.0
            while True:
                pixels = plasma(IMG_WIDTH, IMG_HEIGHT, t)
                frame = build_image_frame(1, 16, IMG_WIDTH, IMG_HEIGHT, pixels)
                ser.write(frame)
                ser.flush()
                # Consume the JSON response
                ser.readline()
                t += 0.3
                frame_count += 1
        except KeyboardInterrupt:
            elapsed = time.monotonic() - t_start
            fps = frame_count / elapsed if elapsed > 0 else 0
            print(f"\n  {frame_count} frames in {elapsed:.1f}s = {fps:.1f} fps")
            print("  (bottleneck is Python-side pixel generation, not transport)")

    print()
    print("Done. Displays should show test patterns.")
    ser.close()


if __name__ == "__main__":
    main()
