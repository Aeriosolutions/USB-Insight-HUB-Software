#!/usr/bin/env python3
"""Live voltage/current graph — polls each channel and renders a
scrolling dual-trace graph on the corresponding display.

Uses the Hub class for reliable JSON communication and raw serial
writes for binary image frames.

Usage:
    python demo_live_graph.py                     # all 3 channels
    python demo_live_graph.py --channels 1        # single channel
    python demo_live_graph.py --channels 1,2      # two channels
    python demo_live_graph.py --port /dev/cu.usbmodemXXX

Ctrl-C to stop.
"""

import argparse
import collections
import json
import struct
import sys
import time

from binary_transport import (
    IMG_FLAG_BUFFER, IMG_FLAG_DIRECT, IMG_FLAG_SPRITE,
    build_image_frame, rgb565_bytes, rle_encode,
)
from hub import Hub, find_hub

MODE_MAP = {"buffer": IMG_FLAG_BUFFER, "sprite": IMG_FLAG_SPRITE, "direct": IMG_FLAG_DIRECT}

# Display geometry
WIDTH = 226
HEIGHT = 90

# Graph layout
MARGIN_LEFT = 32      # voltage Y-axis labels
MARGIN_RIGHT = 32     # current Y-axis labels
MARGIN_TOP = 9        # title row
MARGIN_BOTTOM = 2
GRAPH_W = WIDTH - MARGIN_LEFT - MARGIN_RIGHT
GRAPH_H = HEIGHT - MARGIN_TOP - MARGIN_BOTTOM

# Colors (RGB888 tuples)
BG       = (0, 0, 0)
GRID     = (30, 30, 40)
AXIS     = (35, 35, 45)
V_COLOR  = (255, 220, 50)    # yellow — voltage
I_COLOR  = (50, 220, 255)    # cyan   — current
LABEL    = (120, 120, 130)
CH_COLORS = {
    1: (255, 80, 80),   # red tint for CH1 title
    2: (80, 255, 80),   # green tint for CH2 title
    3: (80, 80, 255),   # blue tint for CH3 title
}

# 3x5 bitmap font — digits, units, punctuation
FONT = {
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
    ' ': [0b000, 0b000, 0b000, 0b000, 0b000],
    'V': [0b101, 0b101, 0b101, 0b101, 0b010],
    'A': [0b010, 0b101, 0b111, 0b101, 0b101],
    'm': [0b000, 0b000, 0b111, 0b101, 0b101],
    'C': [0b111, 0b100, 0b100, 0b100, 0b111],
    'H': [0b101, 0b101, 0b111, 0b101, 0b101],
}


# ── pixel buffer ──────────────────────────────────────────────────

class Framebuf:
    """Minimal RGB565 framebuffer for host-side rendering."""

    __slots__ = ('w', 'h', 'buf')

    def __init__(self, w, h):
        self.w, self.h = w, h
        self.buf = bytearray(w * h * 2)

    def clear(self, r=0, g=0, b=0):
        px = rgb565_bytes(r, g, b)
        self.buf[:] = px * (self.w * self.h)

    def pixel(self, x, y, r, g, b):
        if 0 <= x < self.w and 0 <= y < self.h:
            o = (y * self.w + x) * 2
            self.buf[o:o + 2] = rgb565_bytes(r, g, b)

    def hline(self, x0, x1, y, r, g, b):
        if not (0 <= y < self.h):
            return
        x0, x1 = max(0, x0), min(self.w - 1, x1)
        px = rgb565_bytes(r, g, b)
        for x in range(x0, x1 + 1):
            o = (y * self.w + x) * 2
            self.buf[o:o + 2] = px

    def vline(self, x, y0, y1, r, g, b):
        if not (0 <= x < self.w):
            return
        y0, y1 = max(0, y0), min(self.h - 1, y1)
        px = rgb565_bytes(r, g, b)
        for y in range(y0, y1 + 1):
            o = (y * self.w + x) * 2
            self.buf[o:o + 2] = px

    def line(self, x0, y0, x1, y1, r, g, b):
        """Bresenham's line algorithm."""
        dx = abs(x1 - x0)
        dy = -abs(y1 - y0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx + dy
        while True:
            self.pixel(x0, y0, r, g, b)
            if x0 == x1 and y0 == y1:
                break
            e2 = 2 * err
            if e2 >= dy:
                err += dy
                x0 += sx
            if e2 <= dx:
                err += dx
                y0 += sy

    def text(self, x, y, s, r, g, b):
        for ch in s:
            glyph = FONT.get(ch)
            if glyph:
                for row_i, row in enumerate(glyph):
                    for col in range(3):
                        if row & (1 << (2 - col)):
                            self.pixel(x + col, y + row_i, r, g, b)
            x += 4

    def bytes(self):
        return bytes(self.buf)

    def bytes_rgb332(self):
        """Convert RGB565 buffer to RGB332 (8bpp). Half the size."""
        out = bytearray(self.w * self.h)
        for i in range(self.w * self.h):
            o = i * 2
            raw = self.buf[o] | (self.buf[o + 1] << 8)
            r = (raw >> 11) & 0x1F  # 5 bits
            g = (raw >> 5) & 0x3F   # 6 bits
            b = raw & 0x1F          # 5 bits
            out[i] = (r >> 2 << 5) | (g >> 3 << 2) | (b >> 3)
        return bytes(out)

    def to_image(self, scale=1):
        """Convert to a PIL Image (RGB). Requires Pillow."""
        from PIL import Image
        img = Image.new("RGB", (self.w, self.h))
        pixels = img.load()
        for y in range(self.h):
            for x in range(self.w):
                o = (y * self.w + x) * 2
                raw = self.buf[o] | (self.buf[o + 1] << 8)
                r = (raw >> 11) << 3
                g = ((raw >> 5) & 0x3F) << 2
                b = (raw & 0x1F) << 3
                pixels[x, y] = (r, g, b)
        if scale > 1:
            img = img.resize((self.w * scale, self.h * scale),
                             Image.NEAREST)
        return img


# ── graph rendering ───────────────────────────────────────────────

def nice_range(values, margin=0.1, min_span=None):
    """Auto-range with a bit of padding and optional minimum span.

    If *min_span* is set, the returned range is at least that wide,
    centred on the data midpoint.  This prevents measurement noise from
    filling the entire graph height.
    """
    if not values:
        return 0.0, 100.0
    lo, hi = min(values), max(values)
    span = hi - lo
    if span < 1.0:
        span = max(abs(hi), 1.0)
        lo, hi = hi - span, hi + span * 0.1
    pad = span * margin
    lo, hi = lo - pad, hi + pad
    if min_span is not None and (hi - lo) < min_span:
        mid = (lo + hi) / 2.0
        lo, hi = mid - min_span / 2.0, mid + min_span / 2.0
    return lo, hi


def fmt_mv(val):
    """Format millivolts compactly: '4.99V' or '325mV'."""
    if abs(val) >= 1000:
        return f"{val / 1000:.2f}V"
    return f"{val:.0f}mV"


def fmt_ma(val):
    """Format milliamps compactly."""
    if abs(val) >= 1000:
        return f"{val / 1000:.1f}A"
    return f"{val:.1f}mA"


def render(fb, ch, v_hist, i_hist, axis_mode="min-span"):
    """Render dual-trace graph into framebuffer.

    *axis_mode*: 'auto' (tight), 'min-span' (auto with minimum range),
                 'fixed' (0-5.5V, 0-2A).
    """
    fb.clear(*BG)

    gx = MARGIN_LEFT
    gy = MARGIN_TOP
    gw = GRAPH_W
    gh = GRAPH_H

    # ── title bar ──
    ch_color = CH_COLORS.get(ch, LABEL)
    fb.text(1, 1, f"CH{ch}", *ch_color)

    if v_hist:
        fb.text(24, 1, fmt_mv(v_hist[-1]), *V_COLOR)
    if i_hist:
        fb.text(WIDTH // 2 + 10, 1, fmt_ma(i_hist[-1]), *I_COLOR)

    # ── axes ──
    fb.vline(gx, gy, gy + gh, *AXIS)
    fb.hline(gx, gx + gw, gy + gh, *AXIS)

    # ── grid: 4 horizontal divisions, dashed ──
    for div in range(5):
        row = gy + int(gh * div / 4)
        for x in range(gx + 1, gx + gw, 4):
            fb.pixel(x, row, *GRID)

    # ── grid: 4 vertical divisions, dashed ──
    for div in range(1, 4):
        col = gx + int(gw * div / 4)
        for y in range(gy, gy + gh, 4):
            fb.pixel(col, y, *GRID)

    n = len(v_hist)
    if n < 2:
        return

    if axis_mode == "fixed":
        v_lo, v_hi = 0.0, 5500.0     # 0 – 5.5 V
        i_lo, i_hi = -100.0, 2000.0   # -100 mA – 2 A
    elif axis_mode == "min-span":
        v_lo, v_hi = nice_range(v_hist, min_span=500.0)   # at least 500 mV
        i_lo, i_hi = nice_range(i_hist, min_span=200.0)   # at least 200 mA
    else:
        v_lo, v_hi = nice_range(v_hist)
        i_lo, i_hi = nice_range(i_hist)

    # ── Y-axis labels: voltage (left, yellow) ──
    for tick in range(3):
        frac = tick / 2.0
        val = v_lo + (v_hi - v_lo) * (1.0 - frac)
        label = fmt_mv(val)
        row = gy + int(gh * frac) - 2
        fb.text(1, row, label, *V_COLOR)

    # ── Y-axis labels: current (right, cyan) ──
    right_x = gx + gw + 3
    for tick in range(3):
        frac = tick / 2.0
        val = i_lo + (i_hi - i_lo) * (1.0 - frac)
        label = fmt_ma(val)
        row = gy + int(gh * frac) - 2
        fb.text(right_x, row, label, *I_COLOR)

    # ── right axis border ──
    fb.vline(gx + gw, gy, gy + gh, *AXIS)

    # ── plot traces as connected lines ──
    def val_to_y(val, lo, hi):
        return max(gy, min(gy + gh - 1,
                   gy + gh - 1 - int((val - lo) / (hi - lo) * (gh - 1))))

    prev_vx, prev_vy = None, None
    prev_cx, prev_cy = None, None

    for idx in range(n):
        x = gx + 1 + int((gw - 2) * idx / (n - 1))

        # Voltage trace
        if v_hi > v_lo:
            vy = val_to_y(v_hist[idx], v_lo, v_hi)
            if prev_vx is not None:
                fb.line(prev_vx, prev_vy, x, vy, *V_COLOR)
                # thicken: draw shifted 1px up
                fb.line(prev_vx, prev_vy - 1, x, vy - 1, *V_COLOR)
            prev_vx, prev_vy = x, vy

        # Current trace
        if i_hi > i_lo:
            cy = val_to_y(i_hist[idx], i_lo, i_hi)
            if prev_cx is not None:
                fb.line(prev_cx, prev_cy, x, cy, *I_COLOR)
                fb.line(prev_cx, prev_cy - 1, x, cy - 1, *I_COLOR)
            prev_cx, prev_cy = x, cy


# ── RLE compression analysis ─────────────────────────────────────

class RLEStats:
    """Accumulate RLE compression statistics across frames."""

    def __init__(self):
        self.frames = 0
        self.total_raw = 0
        self.total_rle = 0
        self.best_ratio = 1.0
        self.worst_ratio = 0.0

    def analyze(self, pixels_8bpp):
        """Analyze one frame's 8bpp pixel data. Returns (raw, rle, ratio)."""
        raw_size = len(pixels_8bpp)
        rle_data = rle_encode(pixels_8bpp)
        rle_size = len(rle_data)
        ratio = rle_size / raw_size if raw_size else 1.0

        self.frames += 1
        self.total_raw += raw_size
        self.total_rle += rle_size
        self.best_ratio = min(self.best_ratio, ratio)
        self.worst_ratio = max(self.worst_ratio, ratio)

        return raw_size, rle_size, ratio

    def summary(self):
        if self.frames == 0:
            return "No frames analyzed"
        avg = self.total_rle / self.total_raw
        saving_pct = (1.0 - avg) * 100
        return (
            f"RLE over {self.frames} frames: "
            f"avg {avg:.2f}x ({saving_pct:+.1f}% size), "
            f"best {self.best_ratio:.2f}x, worst {self.worst_ratio:.2f}x, "
            f"raw {self.total_raw // 1024}KB total → "
            f"rle {self.total_rle // 1024}KB"
        )


# ── serial helpers ────────────────────────────────────────────────

def send_image(hub, ch, fb, bpp=8, flags=IMG_FLAG_SPRITE, rle_stats=None,
               compress=False):
    """Send an image frame and read the response.

    Uses 8bpp RGB332 by default (half the data, ~2x FPS for graphs).
    If *rle_stats* is provided, runs RLE analysis on the pixel data.
    If *compress* is True, RLE-compresses the pixel data.
    """
    if bpp == 8:
        pixels = fb.bytes_rgb332()
    else:
        pixels = fb.bytes()

    if rle_stats is not None:
        rle_stats.analyze(pixels)

    frame = build_image_frame(ch, bpp, WIDTH, HEIGHT, pixels, flags=flags,
                              compress=compress)

    # Write the binary frame
    hub.ser.write(frame)
    hub.ser.flush()

    # Read response — use a short timeout to avoid hanging
    old_timeout = hub.ser.timeout
    hub.ser.timeout = 1.0
    try:
        line = hub.ser.readline().decode("utf-8", errors="replace").strip()
        if line:
            try:
                resp = json.loads(line)
                return resp.get("status") == "ok", resp
            except json.JSONDecodeError:
                # Got non-JSON — might be debug output. Drain and retry.
                hub.ser.reset_input_buffer()
                return False, {"error": "non-json", "raw": line[:80]}
        return False, {"error": "timeout"}
    finally:
        hub.ser.timeout = old_timeout


# ── main loop ─────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="Live V/I graph demo")
    parser.add_argument("--port", help="Serial port (auto-detect if omitted)")
    parser.add_argument(
        "--channels", type=str, default="1,2,3",
        help="Comma-separated channels (default: 1,2,3)",
    )
    parser.add_argument(
        "--history", type=int, default=GRAPH_W,
        help=f"Samples to keep (default: {GRAPH_W} = graph width)",
    )
    parser.add_argument(
        "--record", type=str, metavar="FILE.gif",
        help="Record frames and save as animated GIF on exit",
    )
    parser.add_argument(
        "--scale", type=int, default=3,
        help="Scale factor for recorded GIF (default: 3)",
    )
    parser.add_argument(
        "--max-frames", type=int, default=200,
        help="Max frames to record per channel (default: 200)",
    )
    parser.add_argument(
        "--mode", choices=sorted(MODE_MAP), default="sprite",
        help="Image write mode (default: sprite)",
    )
    parser.add_argument(
        "--axis", choices=["auto", "fixed", "min-span"], default="fixed",
        help="Y-axis scaling: 'fixed' (0-5.5V, 0-2A), "
             "'min-span' (auto with minimum range, default), "
             "'auto' (tight auto-scale)",
    )
    parser.add_argument(
        "--no-rle", action="store_true", default=False,
        help="Disable RLE compression (for A/B comparison)",
    )
    args = parser.parse_args()

    channels = [int(c) for c in args.channels.split(",")]

    port = args.port or find_hub()
    if not port:
        print("No Insight Hub found. Pass --port or connect a hub.")
        sys.exit(1)

    print(f"Connecting to {port}...")
    hub = Hub(port)
    print(f"Hub connected on {hub.port}")

    # Verify we can read data
    state = hub.get("state")
    if state:
        ver = state.get("cpu_ver", "?")
        print(f"Firmware: {ver}")
    else:
        print("Warning: could not read hub state")

    # No explicit screenLock — each image push sets imageMode with a
    # 10-second auto-timeout. This lets buttons still work: if the user
    # presses a button (menuIsActive), we detect it and exit gracefully.

    # Per-channel history buffers
    v_hist = {ch: collections.deque(maxlen=args.history) for ch in channels}
    i_hist = {ch: collections.deque(maxlen=args.history) for ch in channels}

    fb = Framebuf(WIDTH, HEIGHT)

    # Recording state
    recorded_frames = {ch: [] for ch in channels}
    recording = args.record is not None

    img_flags = MODE_MAP[args.mode]
    bpp = 8
    if img_flags == IMG_FLAG_SPRITE:
        bpp = 8  # sprite requires 8bpp
    use_rle = not args.no_rle

    print(f"Graphing CH{','.join(str(c) for c in channels)} "
          f"mode={args.mode} bpp={bpp} axis={args.axis} "
          f"rle={'on' if use_rle else 'off'} "
          "— Ctrl-C or press a hub button to stop")
    if recording:
        print(f"Recording up to {args.max_frames} frames per channel "
              f"(scale {args.scale}x) → {args.record}")
    print()

    rle_stats = RLEStats()

    frame_count = 0
    t_start = time.monotonic()
    errors = 0
    menu_check_interval = 10  # check menuIsActive every N frames
    stopped_by_button = False

    try:
        while True:
            # ── check for button press periodically ──
            if frame_count % menu_check_interval == 0:
                state = hub.get("menuIsActive")
                if state and state.get("menuIsActive"):
                    stopped_by_button = True
                    print("\n\n  Button press detected — returning control to hub.")
                    break

            # ── poll all channels via Hub.get() ──
            for ch in channels:
                data = hub.get(f"CH{ch}")
                if data:
                    ch_data = data.get(f"CH{ch}", {})
                    v = ch_data.get("voltage")
                    i = ch_data.get("current")
                    if v is not None and i is not None:
                        v_hist[ch].append(float(v))
                        i_hist[ch].append(float(i))

            # ── render and push each channel ──
            for ch in channels:
                if len(v_hist[ch]) < 2:
                    continue

                render(fb, ch, list(v_hist[ch]), list(i_hist[ch]),
                       axis_mode=args.axis)

                if recording and len(recorded_frames[ch]) < args.max_frames:
                    recorded_frames[ch].append(fb.to_image(args.scale))

                ok, resp = send_image(hub, ch, fb, bpp=bpp, flags=img_flags,
                                      rle_stats=rle_stats, compress=use_rle)
                if not ok:
                    errors += 1
                    if errors <= 5:
                        print(f"\n  Image CH{ch} error: {resp}")
                frame_count += 1

            # ── status line ──
            if frame_count % (5 * len(channels)) == 0 and frame_count > 0:
                elapsed = time.monotonic() - t_start
                fps = frame_count / elapsed
                parts = []
                for ch in channels:
                    if v_hist[ch]:
                        parts.append(
                            f"CH{ch}: {fmt_mv(v_hist[ch][-1])} "
                            f"{fmt_ma(i_hist[ch][-1])}"
                        )
                status = " | ".join(parts)
                print(f"\r  {fps:.1f} fps  {status}"
                      f"{'  err=' + str(errors) if errors else ''}"
                      "    ", end="", flush=True)

    except KeyboardInterrupt:
        pass

    elapsed = time.monotonic() - t_start
    fps = frame_count / elapsed if elapsed > 0 else 0
    print(f"\n  {frame_count} frames in {elapsed:.1f}s = {fps:.1f} fps "
          f"({fps / len(channels):.1f} per channel)")
    if errors:
        print(f"  {errors} errors")
    print(f"  {rle_stats.summary()}")

    # Save recorded GIF
    if recording:
        from PIL import Image

        all_frames = []
        for ch in channels:
            all_frames.extend(recorded_frames[ch])

        if len(channels) > 1:
            # Stack channels vertically with a 2px gap
            gap = 2 * args.scale
            combined = []
            max_len = max(len(recorded_frames[ch]) for ch in channels)
            for i in range(max_len):
                imgs = []
                for ch in channels:
                    if i < len(recorded_frames[ch]):
                        imgs.append(recorded_frames[ch][i])
                    elif recorded_frames[ch]:
                        imgs.append(recorded_frames[ch][-1])
                if not imgs:
                    continue
                total_h = sum(im.height for im in imgs) + gap * (len(imgs) - 1)
                canvas = Image.new("RGB", (imgs[0].width, total_h), (0, 0, 0))
                y = 0
                for im in imgs:
                    canvas.paste(im, (0, y))
                    y += im.height + gap
                combined.append(canvas)
            all_frames = combined

        if all_frames:
            # ~2 fps playback to match real speed
            frame_ms = int(1000 / max(fps / len(channels), 0.5))
            all_frames[0].save(
                args.record,
                save_all=True,
                append_images=all_frames[1:],
                duration=frame_ms,
                loop=0,
                optimize=True,
            )
            print(f"  Saved {len(all_frames)} frames → {args.record} "
                  f"({frame_ms}ms/frame)")
        else:
            print("  No frames to save.")

    hub.close()
    print("  Done.")


if __name__ == "__main__":
    main()
