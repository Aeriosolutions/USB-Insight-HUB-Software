#!/usr/bin/env python3
"""Live meter graph demo — streams V/I data from the hub and renders
a scrolling graph back to the display.

Usage:
    python demo_meter_graph.py [--port /dev/cu.usbmodem...] [--channel 1]
    python demo_meter_graph.py --all          # graph on all 3 displays
    python demo_meter_graph.py --channel 2 --interval 50

Ctrl-C to stop.

Requires: pyserial
"""

import argparse
import collections
import json
import struct
import sys
import threading
import time

import serial

from binary_transport import (
    BIN_CMD_METER_STREAM,
    build_image_frame,
    build_meter_subscribe_frame,
    crc32,
    parse_frame,
    parse_meter_sample,
    rgb565,
)
from hub import find_hub

# Display geometry
IMG_WIDTH = 226
IMG_HEIGHT = 90

# Graph layout
GRAPH_LEFT = 30       # space for Y-axis labels
GRAPH_RIGHT = 2
GRAPH_TOP = 10        # space for title
GRAPH_BOTTOM = 10     # space for X-axis
GRAPH_W = IMG_WIDTH - GRAPH_LEFT - GRAPH_RIGHT
GRAPH_H = IMG_HEIGHT - GRAPH_TOP - GRAPH_BOTTOM

# Colors (RGB888)
COLOR_BG = (0, 0, 0)
COLOR_GRID = (40, 40, 40)
COLOR_AXIS = (80, 80, 80)
COLOR_VOLTAGE = (255, 220, 50)    # yellow
COLOR_CURRENT = (50, 220, 255)    # cyan
COLOR_TEXT = (160, 160, 160)
COLOR_TITLE_V = COLOR_VOLTAGE
COLOR_TITLE_I = COLOR_CURRENT

# Tiny 3x5 font for axis labels (digits 0-9, '.', '-', 'V', 'A', 'm', 'k', ' ')
_FONT_3X5 = {
    '0': [0b111, 0b101, 0b101, 0b101, 0b111],
    '1': [0b010, 0b110, 0b010, 0b010, 0b111],
    '2': [0b111, 0b001, 0b111, 0b100, 0b111],
    '3': [0b111, 0b001, 0b111, 0b001, 0b111],
    '4': [0b101, 0b101, 0b111, 0b001, 0b001],
    '5': [0b111, 0b100, 0b111, 0b001, 0b111],
    '6': [0b111, 0b100, 0b111, 0b101, 0b111],
    '7': [0b111, 0b001, 0b010, 0b010, 0b010],
    '8': [0b111, 0b101, 0b111, 0b101, 0b111],
    '9': [0b111, 0b101, 0b111, 0b001, 0b111],
    '.': [0b000, 0b000, 0b000, 0b000, 0b010],
    '-': [0b000, 0b000, 0b111, 0b000, 0b000],
    'V': [0b101, 0b101, 0b101, 0b101, 0b010],
    'A': [0b010, 0b101, 0b111, 0b101, 0b101],
    'm': [0b000, 0b000, 0b111, 0b101, 0b101],
    'k': [0b100, 0b101, 0b110, 0b101, 0b101],
    ' ': [0b000, 0b000, 0b000, 0b000, 0b000],
}


class PixelBuffer:
    """Simple 2D pixel buffer for RGB565 rendering."""

    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.buf = bytearray(width * height * 2)

    def fill(self, r, g, b):
        pixel = struct.pack("<H", rgb565(r, g, b))
        self.buf = bytearray(pixel * (self.width * self.height))

    def set_pixel(self, x, y, r, g, b):
        if 0 <= x < self.width and 0 <= y < self.height:
            offset = (y * self.width + x) * 2
            struct.pack_into("<H", self.buf, offset, rgb565(r, g, b))

    def hline(self, x0, x1, y, r, g, b):
        if y < 0 or y >= self.height:
            return
        x0 = max(0, x0)
        x1 = min(self.width - 1, x1)
        pixel = struct.pack("<H", rgb565(r, g, b))
        for x in range(x0, x1 + 1):
            offset = (y * self.width + x) * 2
            self.buf[offset:offset + 2] = pixel

    def vline(self, x, y0, y1, r, g, b):
        if x < 0 or x >= self.width:
            return
        y0 = max(0, y0)
        y1 = min(self.height - 1, y1)
        pixel = struct.pack("<H", rgb565(r, g, b))
        for y in range(y0, y1 + 1):
            offset = (y * self.width + x) * 2
            self.buf[offset:offset + 2] = pixel

    def draw_char(self, x, y, ch, r, g, b):
        glyph = _FONT_3X5.get(ch)
        if not glyph:
            return
        for row_idx, row in enumerate(glyph):
            for col in range(3):
                if row & (1 << (2 - col)):
                    self.set_pixel(x + col, y + row_idx, r, g, b)

    def draw_text(self, x, y, text, r, g, b):
        for ch in text:
            self.draw_char(x, y, ch, r, g, b)
            x += 4  # 3px glyph + 1px spacing

    def to_bytes(self):
        return bytes(self.buf)


def format_value(val, unit):
    """Format a meter value compactly for axis labels."""
    if abs(val) >= 1000:
        return f"{val / 1000:.1f}{unit}"
    return f"{val:.0f}"


def auto_range(values, margin=0.1):
    """Compute nice Y-axis range from a list of values."""
    if not values:
        return 0.0, 1.0
    vmin = min(values)
    vmax = max(values)
    span = vmax - vmin
    if span < 0.1:
        span = max(abs(vmax), 1.0)
        vmin = vmax - span
    pad = span * margin
    return vmin - pad, vmax + pad


def render_graph(
    pb, samples_v, samples_i, time_window_s, channel,
    v_range=None, i_range=None,
):
    """Render a dual-trace scrolling graph into the pixel buffer."""
    pb.fill(*COLOR_BG)

    gx = GRAPH_LEFT
    gy = GRAPH_TOP
    gw = GRAPH_W
    gh = GRAPH_H

    # Auto-range
    if v_range is None:
        v_min, v_max = auto_range(list(samples_v))
    else:
        v_min, v_max = v_range
    if i_range is None:
        i_min, i_max = auto_range(list(samples_i))
    else:
        i_min, i_max = i_range

    # Grid — horizontal lines (4 divisions)
    for i in range(5):
        y = gy + int(gh * i / 4)
        for x in range(gx, gx + gw, 3):  # dashed
            pb.set_pixel(x, y, *COLOR_GRID)

    # Grid — vertical lines (time divisions)
    for i in range(1, 4):
        x = gx + int(gw * i / 4)
        for y in range(gy, gy + gh, 3):  # dashed
            pb.set_pixel(x, y, *COLOR_GRID)

    # Axis border
    pb.vline(gx, gy, gy + gh, *COLOR_AXIS)
    pb.hline(gx, gx + gw, gy + gh, *COLOR_AXIS)

    # Title
    title_v = f"{format_value(samples_v[-1] if samples_v else 0, 'V')}V"
    title_i = f"{format_value(samples_i[-1] if samples_i else 0, 'A')}mA"
    pb.draw_text(2, 1, title_v, *COLOR_TITLE_V)
    pb.draw_text(gx + gw // 2, 1, title_i, *COLOR_TITLE_I)

    # Y-axis labels — voltage side (left)
    for i in range(3):
        frac = i / 2.0
        val = v_min + (v_max - v_min) * (1 - frac)
        label = format_value(val, 'k')
        y = gy + int(gh * frac) - 2
        pb.draw_text(1, y, label, *COLOR_VOLTAGE)

    # Plot traces
    n_samples = len(samples_v)
    if n_samples < 2:
        return

    for idx in range(n_samples):
        x = gx + int(gw * idx / (n_samples - 1)) if n_samples > 1 else gx

        # Voltage trace
        v = samples_v[idx]
        if v_max > v_min:
            vy = gy + gh - 1 - int((v - v_min) / (v_max - v_min) * (gh - 1))
            vy = max(gy, min(gy + gh - 1, vy))
            pb.set_pixel(x, vy, *COLOR_VOLTAGE)
            # Thicken: draw +/- 1 pixel vertically
            pb.set_pixel(x, max(gy, vy - 1), *COLOR_VOLTAGE)

        # Current trace
        c = samples_i[idx]
        if i_max > i_min:
            cy = gy + gh - 1 - int((c - i_min) / (i_max - i_min) * (gh - 1))
            cy = max(gy, min(gy + gh - 1, cy))
            pb.set_pixel(x, cy, *COLOR_CURRENT)
            pb.set_pixel(x, max(gy, cy - 1), *COLOR_CURRENT)


class MeterReceiver(threading.Thread):
    """Background thread that reads binary frames from serial."""

    def __init__(self, ser):
        super().__init__(daemon=True)
        self.ser = ser
        self.lock = threading.Lock()
        # Per-channel rolling buffers
        self.voltage = {1: collections.deque(maxlen=500),
                        2: collections.deque(maxlen=500),
                        3: collections.deque(maxlen=500)}
        self.current = {1: collections.deque(maxlen=500),
                        2: collections.deque(maxlen=500),
                        3: collections.deque(maxlen=500)}
        self.timestamps = {1: collections.deque(maxlen=500),
                           2: collections.deque(maxlen=500),
                           3: collections.deque(maxlen=500)}
        self.sample_count = 0
        self.running = True

    def run(self):
        buf = bytearray()
        while self.running:
            try:
                data = self.ser.read(self.ser.in_waiting or 1)
            except Exception:
                break
            if not data:
                continue
            buf.extend(data)

            # Try to parse frames from buf
            while True:
                # Look for frame start (\0)
                idx = buf.find(b'\x00')
                if idx < 0:
                    # No frame start — might be JSON text, discard
                    buf.clear()
                    break
                if idx > 0:
                    # Discard bytes before the frame (JSON responses etc.)
                    del buf[:idx]

                # Need at least escape(1) + header(9) + crc(4) = 14 bytes
                if len(buf) < 14:
                    break

                # Peek at payload length
                length = struct.unpack_from("<I", buf, 6)[0]
                frame_size = 1 + 9 + length + 4

                if len(buf) < frame_size:
                    break  # incomplete frame

                frame_bytes = bytes(buf[:frame_size])
                del buf[:frame_size]

                try:
                    cmd, flags, payload = parse_frame(frame_bytes)
                except ValueError:
                    continue

                if cmd == BIN_CMD_METER_STREAM:
                    try:
                        sample = parse_meter_sample(payload)
                    except ValueError:
                        continue
                    ts = sample["timestamp_ms"]
                    with self.lock:
                        for ch_data in sample["channels"]:
                            ch = ch_data["channel"]
                            if ch in self.voltage:
                                self.voltage[ch].append(ch_data["voltage_mV"])
                                self.current[ch].append(ch_data["current_mA"])
                                self.timestamps[ch].append(ts)
                        self.sample_count += 1

    def get_samples(self, channel, max_points=None):
        """Return (voltages, currents) lists for the given channel."""
        with self.lock:
            v = list(self.voltage.get(channel, []))
            c = list(self.current.get(channel, []))
        if max_points and len(v) > max_points:
            v = v[-max_points:]
            c = c[-max_points:]
        return v, c

    def stop(self):
        self.running = False


def main():
    parser = argparse.ArgumentParser(description="Live meter graph demo")
    parser.add_argument("--port", help="Serial port (auto-detect if omitted)")
    parser.add_argument("--channel", type=int, default=1, choices=[1, 2, 3],
                        help="Channel to display graph on (default: 1)")
    parser.add_argument("--all", action="store_true",
                        help="Show graphs on all 3 displays")
    parser.add_argument("--interval", type=int, default=100,
                        help="Sample interval in ms (default: 100, min: 20)")
    parser.add_argument("--window", type=float, default=10.0,
                        help="Time window in seconds (default: 10)")
    args = parser.parse_args()

    port = args.port or find_hub()
    if not port:
        print("No Insight Hub found. Pass --port or connect a hub.")
        sys.exit(1)

    channels = [1, 2, 3] if args.all else [args.channel]
    # Stream mask: which channels to read from the meter
    stream_mask = 0x07  # always stream all 3 channels (data is tiny)
    display_channels = channels  # which displays to render graphs on

    print(f"Connecting to {port}...")
    ser = serial.Serial(port, 115200, timeout=0.1)
    time.sleep(0.5)
    ser.reset_input_buffer()

    # Quick connectivity check
    ser.write(b'{"action":"get","params":["esp32_ver"]}\n')
    ser.flush()
    line = ser.readline().decode("utf-8", errors="replace").strip()
    if line:
        try:
            resp = json.loads(line)
            ver = resp.get("data", {}).get("cpu_ver", "?")
            print(f"Hub firmware: {ver}")
        except json.JSONDecodeError:
            pass

    # Start receiver thread
    receiver = MeterReceiver(ser)
    receiver.start()

    # Subscribe to meter stream
    frame = build_meter_subscribe_frame(stream_mask, args.interval)
    ser.write(frame)
    ser.flush()
    time.sleep(0.3)  # let the JSON response arrive and be consumed by receiver

    max_points = GRAPH_W  # one sample per pixel column
    pb = PixelBuffer(IMG_WIDTH, IMG_HEIGHT)

    print(f"Streaming CH{','.join(str(c) for c in display_channels)} "
          f"every {args.interval}ms, {args.window}s window")
    print("Ctrl-C to stop")
    print()

    frame_count = 0
    t_start = time.monotonic()

    try:
        while True:
            for disp_ch in display_channels:
                # Each display shows its own channel's data
                v_samples, i_samples = receiver.get_samples(disp_ch, max_points)

                if len(v_samples) < 2:
                    time.sleep(0.05)
                    continue

                render_graph(pb, v_samples, i_samples, args.window, disp_ch)
                pixels = pb.to_bytes()
                img_frame = build_image_frame(
                    disp_ch, 16, IMG_WIDTH, IMG_HEIGHT, pixels
                )
                ser.write(img_frame)
                ser.flush()
                # The JSON response from the image command will be consumed
                # by the receiver thread (it discards non-meter frames)
                frame_count += 1

            # Print stats periodically
            elapsed = time.monotonic() - t_start
            if frame_count % 20 == 0 and frame_count > 0:
                fps = frame_count / elapsed
                sc = receiver.sample_count
                v_last, i_last = receiver.get_samples(display_channels[0])
                v_str = f"{v_last[-1]:.0f}mV" if v_last else "?"
                i_str = f"{i_last[-1]:.1f}mA" if i_last else "?"
                print(f"\r  {sc} samples, {frame_count} frames "
                      f"({fps:.1f} fps), CH{display_channels[0]}: "
                      f"{v_str} {i_str}    ", end="", flush=True)

            # Pace: don't render faster than ~15fps per display
            time.sleep(max(0.06, args.interval / 1000.0))

    except KeyboardInterrupt:
        elapsed = time.monotonic() - t_start
        fps = frame_count / elapsed if elapsed > 0 else 0
        print(f"\n\n  {frame_count} frames in {elapsed:.1f}s = {fps:.1f} fps")
        print(f"  {receiver.sample_count} meter samples received")

    # Stop streaming
    print("  Stopping meter stream...")
    stop_frame = build_meter_subscribe_frame(0, 0)
    ser.write(stop_frame)
    ser.flush()
    time.sleep(0.2)

    receiver.stop()
    receiver.join(timeout=1.0)
    ser.close()
    print("  Done.")


if __name__ == "__main__":
    main()
