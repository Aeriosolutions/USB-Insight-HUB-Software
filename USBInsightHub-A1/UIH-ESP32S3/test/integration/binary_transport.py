"""Binary transport protocol for the Insight Hub.

Builds and sends binary frames using SOH-escaped protocol:

    SOH  version(1)  cmd(2,LE)  flags(2,LE)  length(4,LE)  payload(length)  crc32(4,LE)

SOH (0x01, Start of Header) is the binary escape byte.  It cannot appear
in valid JSON text (RFC 8259 requires control chars U+0000–U+001F to be
escaped), so it unambiguously signals a binary frame on a shared
text/binary serial channel.

All multi-byte fields are little-endian.  CRC-32 is computed over
[version..payload] (everything after the SOH escape).
"""

import struct
import zlib

BIN_ESCAPE = 0x01  # SOH — binary frame escape byte
BIN_PROTOCOL_VERSION = 0x01
BIN_CMD_IMAGE = 0x0001
BIN_CMD_ECHO = 0x0002
BIN_CMD_METER_STREAM = 0x0003
BIN_CMD_SCREEN_LOCK = 0x0004
BIN_CMD_SCREEN_READY = 0x0005

# Image write mode flags (passed in frame flags field)
IMG_FLAG_BUFFER = 0   # buffer pixels → flush after CRC (default)
IMG_FLAG_SPRITE = 1   # write into TFT_eSprite (8bpp only)
IMG_FLAG_DIRECT = 2   # stream directly to SPI (no buffer, CRC-unsafe)
IMG_FLAG_RLE    = 0x04  # bit 2: payload is RLE-compressed (count+value pairs)


def crc32(data: bytes, crc: int = 0) -> int:
    """Compute CRC-32 matching ESP32's esp_crc32_le(crc, data, len).

    esp_crc32_le uses standard CRC-32 (init=~crc, final=~reg),
    which is equivalent to zlib.crc32.
    """
    return zlib.crc32(data, crc) & 0xFFFFFFFF


def build_frame(cmd: int, payload: bytes, flags: int = 0) -> bytes:
    """Build a complete binary frame ready to send over serial.

    CRC-32 is computed over header+payload (everything after the \\0 escape).
    """
    header = struct.pack(
        "<BHHI",
        BIN_PROTOCOL_VERSION,
        cmd,
        flags,
        len(payload),
    )
    checksummed = header + payload
    checksum = crc32(checksummed)
    return bytes([BIN_ESCAPE]) + checksummed + struct.pack("<I", checksum)


def parse_frame(data: bytes) -> tuple:
    """Parse a binary frame. Returns (cmd, flags, payload) or raises ValueError."""
    if len(data) < 14 or data[0] != BIN_ESCAPE:
        raise ValueError("not a binary frame")
    version = data[1]
    if version != BIN_PROTOCOL_VERSION:
        raise ValueError(f"unsupported version {version}")
    cmd, flags, length = struct.unpack_from("<HHI", data, 2)
    if len(data) < 10 + length + 4:
        raise ValueError("frame too short")
    payload = data[10:10 + length]
    expected_crc = struct.unpack_from("<I", data, 10 + length)[0]
    actual_crc = crc32(data[1:10 + length])
    if expected_crc != actual_crc:
        raise ValueError(f"CRC mismatch: expected 0x{expected_crc:08X}, got 0x{actual_crc:08X}")
    return cmd, flags, payload


def rle_encode(data: bytes) -> bytes:
    """RLE-encode a bytes object: [count][value] pairs, max run 255."""
    if not data:
        return b""
    out = bytearray()
    prev = data[0]
    count = 1
    for b in data[1:]:
        if b == prev and count < 255:
            count += 1
        else:
            out.append(count)
            out.append(prev)
            prev = b
            count = 1
    out.append(count)
    out.append(prev)
    return bytes(out)


def build_image_frame(
    port: int,
    bpp: int,
    width: int,
    height: int,
    pixel_data: bytes,
    flags: int = 0,
    compress: bool = False,
) -> bytes:
    """Build a binary frame for the image command (cmd=0x0001).

    *pixel_data* should be raw pixel bytes:
      - 16bpp: little-endian RGB565, width*height*2 bytes
      - 8bpp:  RGB332, width*height bytes

    If *compress* is True, pixel_data is RLE-encoded and IMG_FLAG_RLE
    is ORed into *flags*.
    """
    if compress:
        pixel_data = rle_encode(pixel_data)
        flags |= IMG_FLAG_RLE
    sub_header = struct.pack("<BBHH", port, bpp, width, height)
    payload = sub_header + pixel_data
    return build_frame(BIN_CMD_IMAGE, payload, flags)


def build_echo_frame(payload: bytes, flags: int = 0) -> bytes:
    """Build a binary frame for the echo command (cmd=0x0002)."""
    return build_frame(BIN_CMD_ECHO, payload, flags)


def build_meter_subscribe_frame(
    channel_mask: int, interval_ms: int, flags: int = 0
) -> bytes:
    """Build a meter stream subscribe frame (cmd=0x0003).

    *channel_mask*: bits 0-2 for CH1-CH3 (e.g. 0x07 = all three).
    Set mask=0 to stop streaming.
    *interval_ms*: sample interval in milliseconds (min 20, max 10000).
    """
    payload = struct.pack("<BH", channel_mask, interval_ms)
    return build_frame(BIN_CMD_METER_STREAM, payload, flags)


def parse_meter_sample(payload: bytes) -> dict:
    """Parse a meter stream sample payload.

    Returns dict with 'timestamp_ms' and 'channels' list of
    {'channel': int, 'voltage_mV': float, 'current_mA': float}.
    """
    if len(payload) < 5:
        raise ValueError("meter sample too short")
    timestamp_ms = struct.unpack_from("<I", payload, 0)[0]
    num_ch = payload[4]
    channels = []
    offset = 5
    for _ in range(num_ch):
        if offset + 9 > len(payload):
            raise ValueError("meter sample truncated")
        ch = payload[offset]
        voltage = struct.unpack_from("<f", payload, offset + 1)[0]
        current = struct.unpack_from("<f", payload, offset + 5)[0]
        channels.append({
            "channel": ch,
            "voltage_mV": voltage,
            "current_mA": current,
        })
        offset += 9
    return {"timestamp_ms": timestamp_ms, "channels": channels}


def build_screen_lock_frame(mask: int, action: int, flags: int = 0) -> bytes:
    """Build a screen lock frame (cmd=0x0004).

    *mask*: channel bitmask (bits 0-2 = CH1-CH3).
    *action*: 1 = lock, 0 = unlock.
    """
    payload = struct.pack("<BB", mask, action)
    return build_frame(BIN_CMD_SCREEN_LOCK, payload, flags)


def build_screen_ready_frame(channel: int, flags: int = 0) -> bytes:
    """Build a screen ready frame (cmd=0x0005).

    *channel*: 1-3.  Response is deferred until the display's render slot.
    """
    return build_frame(BIN_CMD_SCREEN_READY, bytes([channel]), flags)


def rgb565(r: int, g: int, b: int) -> int:
    """Convert 8-bit RGB to 16-bit RGB565.

    Returns the standard RGB565 value (R in high bits, B in low bits).
    """
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def rgb565_bytes(r: int, g: int, b: int) -> bytes:
    """Convert 8-bit RGB to 2-byte RGB565 in display byte order.

    The ESP32 SPI peripheral sends bytes MSB-first, and the ESP32 is
    little-endian, so a uint16_t 0xF800 in memory is [0x00, 0xF8] which
    gets sent as 0x00,0xF8 — wrong.  We need the high byte first in
    memory so SPI sends it correctly.  Pack as big-endian.
    """
    return struct.pack(">H", rgb565(r, g, b))


def solid_image_rgb565(width: int, height: int, r: int, g: int, b: int) -> bytes:
    """Generate a solid-color RGB565 image."""
    pixel = rgb565_bytes(r, g, b)
    return pixel * (width * height)


def gradient_image_rgb565(width: int, height: int) -> bytes:
    """Generate a horizontal red->blue gradient RGB565 image."""
    pixels = bytearray()
    for y in range(height):
        for x in range(width):
            r = int(255 * (1 - x / width))
            b = int(255 * x / width)
            g = int(128 * y / height)
            pixels += rgb565_bytes(r, g, b)
    return bytes(pixels)
