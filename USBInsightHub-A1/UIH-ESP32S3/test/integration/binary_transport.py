"""Binary transport protocol for the Insight Hub.

Builds and sends binary frames using the \0-escaped protocol:

    \0  version(1)  cmd(2,LE)  flags(2,LE)  length(4,LE)  payload(length)  crc32(4,LE)

All multi-byte fields are little-endian.  CRC-32 is computed over
[version..payload] (everything after the \0 escape).
"""

import struct

BIN_PROTOCOL_VERSION = 0x01
BIN_CMD_IMAGE = 0x0001
BIN_CMD_ECHO = 0x0002

# CRC-32 lookup table (polynomial 0xEDB88320, same as ESP32 ROM)
_CRC32_TABLE = []
for _i in range(256):
    _crc = _i
    for _ in range(8):
        if _crc & 1:
            _crc = (_crc >> 1) ^ 0xEDB88320
        else:
            _crc >>= 1
    _CRC32_TABLE.append(_crc)


def crc32(data: bytes, crc: int = 0) -> int:
    """Compute raw CRC-32 matching ESP32's esp_crc32_le(0, data, len).

    Uses init=0, no final XOR — NOT the same as zlib.crc32.
    """
    for b in data:
        crc = _CRC32_TABLE[(crc ^ b) & 0xFF] ^ (crc >> 8)
    return crc & 0xFFFFFFFF


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
    return b"\x00" + checksummed + struct.pack("<I", checksum)


def parse_frame(data: bytes) -> tuple:
    """Parse a binary frame. Returns (cmd, flags, payload) or raises ValueError."""
    if len(data) < 14 or data[0] != 0x00:
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


def build_image_frame(
    port: int,
    bpp: int,
    width: int,
    height: int,
    pixel_data: bytes,
    flags: int = 0,
) -> bytes:
    """Build a binary frame for the image command (cmd=0x0001).

    *pixel_data* should be raw pixel bytes:
      - 16bpp: little-endian RGB565, width*height*2 bytes
      - 8bpp:  RGB332, width*height bytes
    """
    sub_header = struct.pack("<BBHH", port, bpp, width, height)
    payload = sub_header + pixel_data
    return build_frame(BIN_CMD_IMAGE, payload, flags)


def build_echo_frame(payload: bytes, flags: int = 0) -> bytes:
    """Build a binary frame for the echo command (cmd=0x0002)."""
    return build_frame(BIN_CMD_ECHO, payload, flags)


def rgb565(r: int, g: int, b: int) -> int:
    """Convert 8-bit RGB to 16-bit RGB565."""
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def solid_image_rgb565(width: int, height: int, r: int, g: int, b: int) -> bytes:
    """Generate a solid-color RGB565 image."""
    pixel = struct.pack("<H", rgb565(r, g, b))
    return pixel * (width * height)


def gradient_image_rgb565(width: int, height: int) -> bytes:
    """Generate a horizontal red->blue gradient RGB565 image."""
    pixels = bytearray()
    for y in range(height):
        for x in range(width):
            r = int(255 * (1 - x / width))
            b = int(255 * x / width)
            g = int(128 * y / height)
            pixels += struct.pack("<H", rgb565(r, g, b))
    return bytes(pixels)
