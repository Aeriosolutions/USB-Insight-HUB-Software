"""Test binary transport echo command.

Sends payloads via the echo command (cmd=0x0002) and verifies
the device echoes them back as binary frames with matching content.

Usage:
    pytest test_binary_echo.py -v -s
"""

import logging
import time

import pytest

from binary_transport import build_echo_frame, parse_frame, BIN_CMD_ECHO

log = logging.getLogger("test_binary_echo")


@pytest.fixture
def hub(hub_connection):
    """Use the shared hub fixture from conftest."""
    return hub_connection


def send_and_receive_echo(hub, payload: bytes, timeout: float = 2.0) -> bytes:
    """Send an echo frame and read the binary response frame."""
    frame = build_echo_frame(payload)
    log.info("Sending echo (%d byte payload, %d byte frame)", len(payload), len(frame))

    hub.ser.write(frame)
    hub.ser.flush()

    # Read response — binary frame starts with \0
    # Read until we have enough data (header + payload + crc)
    expected_size = 1 + 9 + len(payload) + 4  # \0 + header + payload + crc32
    response = b""
    deadline = time.monotonic() + timeout
    while len(response) < expected_size and time.monotonic() < deadline:
        chunk = hub.ser.read(expected_size - len(response))
        if chunk:
            response += chunk
        else:
            time.sleep(0.01)

    if len(response) < expected_size:
        raise TimeoutError(
            f"Expected {expected_size} bytes, got {len(response)}: {response.hex()}"
        )

    cmd, flags, echoed_payload = parse_frame(response[:expected_size])
    assert cmd == BIN_CMD_ECHO
    return echoed_payload


class TestBinaryEcho:
    """Test echo command for round-trip protocol verification."""

    def test_small_payload(self, hub):
        """Echo a small payload."""
        payload = b"Hello, Insight Hub!"
        result = send_and_receive_echo(hub, payload)
        assert result == payload

    def test_empty_payload(self, hub):
        """Echo with zero-length payload."""
        result = send_and_receive_echo(hub, b"")
        assert result == b""

    def test_binary_payload(self, hub):
        """Echo binary data including null bytes."""
        payload = bytes(range(256))
        result = send_and_receive_echo(hub, payload)
        assert result == payload

    def test_1k_payload(self, hub):
        """Echo 1KB of data."""
        payload = bytes(i & 0xFF for i in range(1024))
        result = send_and_receive_echo(hub, payload)
        assert result == payload

    def test_json_still_works_after_echo(self, hub):
        """Verify JSON API is unaffected after binary echo."""
        payload = b"test"
        send_and_receive_echo(hub, payload)

        time.sleep(0.1)

        data = hub.get("hubMode")
        assert data is not None
        assert "hubMode" in data
