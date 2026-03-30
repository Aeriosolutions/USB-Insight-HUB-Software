"""Unit tests for binary_transport frame building and parsing.

These tests run locally — no device needed.

Usage:
    pytest test_binary_transport.py -v
"""

import struct

import pytest

from binary_transport import (
    BIN_CMD_ECHO,
    BIN_CMD_IMAGE,
    BIN_CMD_METER_STREAM,
    BIN_ESCAPE,
    BIN_PROTOCOL_VERSION,
    build_echo_frame,
    build_frame,
    build_image_frame,
    build_meter_subscribe_frame,
    crc32,
    parse_frame,
    parse_meter_sample,
    rgb565,
    solid_image_rgb565,
)


class TestEscapeByte:
    """Verify SOH (0x01) is used as the binary escape."""

    def test_escape_is_soh(self):
        assert BIN_ESCAPE == 0x01

    def test_frame_starts_with_soh(self):
        frame = build_frame(BIN_CMD_ECHO, b"")
        assert frame[0] == 0x01

    def test_frame_does_not_start_with_null(self):
        frame = build_frame(BIN_CMD_ECHO, b"test")
        assert frame[0] != 0x00


class TestCRC32:
    """Verify CRC-32 matches ESP32's esp_crc32_le(0, data, len)."""

    def test_empty(self):
        assert crc32(b"") == 0x00000000

    def test_known_value(self):
        # "123456789" → 0xCBF43926 with standard CRC-32 (zlib)
        # esp_crc32_le(0, ...) is equivalent to zlib.crc32
        assert crc32(b"123456789") == 0xCBF43926

    def test_incremental(self):
        """CRC can be computed incrementally."""
        data = b"Hello, World!"
        full = crc32(data)
        partial = crc32(data[:5])
        incremental = crc32(data[5:], partial)
        assert incremental == full


class TestBuildFrame:
    """Test frame construction."""

    def test_empty_payload(self):
        frame = build_frame(BIN_CMD_ECHO, b"")
        # SOH(1) + header(9) + CRC(4) = 14 bytes
        assert len(frame) == 14
        assert frame[0] == BIN_ESCAPE

    def test_header_fields(self):
        frame = build_frame(0x1234, b"AB", flags=0x5678)
        assert frame[0] == BIN_ESCAPE
        # version
        assert frame[1] == BIN_PROTOCOL_VERSION
        # cmd (LE)
        assert struct.unpack_from("<H", frame, 2) == (0x1234,)
        # flags (LE)
        assert struct.unpack_from("<H", frame, 4) == (0x5678,)
        # length (LE)
        assert struct.unpack_from("<I", frame, 6) == (2,)
        # payload
        assert frame[10:12] == b"AB"

    def test_payload_with_soh_bytes(self):
        """Payload containing SOH bytes should work — no escaping within frames."""
        payload = bytes([0x01, 0x01, 0x01])
        frame = build_frame(BIN_CMD_ECHO, payload)
        cmd, flags, parsed = parse_frame(frame)
        assert parsed == payload

    def test_payload_with_null_bytes(self):
        """Payload containing null bytes should work."""
        payload = b"\x00\x00\x00"
        frame = build_frame(BIN_CMD_ECHO, payload)
        cmd, flags, parsed = parse_frame(frame)
        assert parsed == payload

    def test_all_byte_values_in_payload(self):
        """All 256 byte values should round-trip."""
        payload = bytes(range(256))
        frame = build_frame(BIN_CMD_ECHO, payload)
        cmd, flags, parsed = parse_frame(frame)
        assert parsed == payload


class TestParseFrame:
    """Test frame parsing."""

    def test_roundtrip(self):
        payload = b"test data"
        frame = build_frame(BIN_CMD_ECHO, payload)
        cmd, flags, parsed = parse_frame(frame)
        assert cmd == BIN_CMD_ECHO
        assert flags == 0
        assert parsed == payload

    def test_too_short(self):
        with pytest.raises(ValueError, match="not a binary frame"):
            parse_frame(b"\x01\x00")

    def test_wrong_escape(self):
        frame = build_frame(BIN_CMD_ECHO, b"")
        bad = b"\x00" + frame[1:]  # replace SOH with NULL
        with pytest.raises(ValueError, match="not a binary frame"):
            parse_frame(bad)

    def test_bad_version(self):
        frame = bytearray(build_frame(BIN_CMD_ECHO, b""))
        frame[1] = 0xFF  # corrupt version
        with pytest.raises(ValueError, match="unsupported version"):
            parse_frame(bytes(frame))

    def test_bad_checksum(self):
        frame = bytearray(build_frame(BIN_CMD_ECHO, b"test"))
        frame[-1] ^= 0xFF  # corrupt CRC
        with pytest.raises(ValueError, match="CRC mismatch"):
            parse_frame(bytes(frame))

    def test_truncated_payload(self):
        frame = build_frame(BIN_CMD_ECHO, b"hello world, this is longer")
        # Keep enough for header (14+) but cut payload short
        with pytest.raises(ValueError, match="frame too short"):
            parse_frame(frame[:16])


class TestImageFrame:
    """Test image frame construction."""

    def test_image_subheader(self):
        pixels = solid_image_rgb565(4, 2, 255, 0, 0)
        frame = build_image_frame(1, 16, 4, 2, pixels)
        cmd, flags, payload = parse_frame(frame)
        assert cmd == BIN_CMD_IMAGE
        # Sub-header: port(1) + bpp(1) + width(2,LE) + height(2,LE)
        port, bpp = payload[0], payload[1]
        width, height = struct.unpack_from("<HH", payload, 2)
        assert port == 1
        assert bpp == 16
        assert width == 4
        assert height == 2
        assert len(payload) == 6 + 4 * 2 * 2  # subheader + pixels

    def test_rgb565_red(self):
        assert rgb565(255, 0, 0) == 0xF800

    def test_rgb565_green(self):
        assert rgb565(0, 255, 0) == 0x07E0

    def test_rgb565_blue(self):
        assert rgb565(0, 0, 255) == 0x001F


class TestMeterFrame:
    """Test meter stream frame construction and parsing."""

    def test_subscribe_all_channels(self):
        frame = build_meter_subscribe_frame(0x07, 100)
        cmd, flags, payload = parse_frame(frame)
        assert cmd == BIN_CMD_METER_STREAM
        mask, interval = struct.unpack_from("<BH", payload, 0)
        assert mask == 0x07
        assert interval == 100

    def test_unsubscribe(self):
        frame = build_meter_subscribe_frame(0x00, 0)
        cmd, flags, payload = parse_frame(frame)
        assert cmd == BIN_CMD_METER_STREAM
        assert payload[0] == 0x00

    def test_parse_sample(self):
        # Build a sample payload: timestamp(4) + num_channels(1) + channel data
        ts = 12345678
        v1, c1 = 5.05, 102.3
        payload = struct.pack("<IB", ts, 1)
        payload += struct.pack("<Bff", 1, v1, c1)
        result = parse_meter_sample(payload)
        assert result["timestamp_ms"] == ts
        assert len(result["channels"]) == 1
        assert result["channels"][0]["channel"] == 1
        assert abs(result["channels"][0]["voltage_mV"] - v1) < 0.01
        assert abs(result["channels"][0]["current_mA"] - c1) < 0.1
