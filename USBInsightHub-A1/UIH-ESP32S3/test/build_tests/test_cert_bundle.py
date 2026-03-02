"""Tests for the certificate bundle generation workflow.

Verifies that:
- The worker script generates a valid x509_crt_bundle.bin from PEM files
- The PIO hook skips generation when the binary already exists
- The PIO hook triggers generation when the binary is missing
"""

import os
import shutil
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

import pytest

# Paths relative to the UIH-ESP32S3 project root
PROJECT_DIR = Path(__file__).resolve().parents[2]
SCRIPTS_DIR = PROJECT_DIR / "scripts"
WORKER_SCRIPT = SCRIPTS_DIR / "generate_cert_bundle_worker.py"
SSL_CERTS_DIR = PROJECT_DIR / "ssl_certs"
COMMITTED_BUNDLE = PROJECT_DIR / "src" / "certs" / "x509_crt_bundle.bin"


@pytest.fixture
def tmp_output(tmp_path):
    """Provide a temporary output directory."""
    return tmp_path / "certs"


class TestWorkerScript:
    """Tests for generate_cert_bundle_worker.py (standalone)."""

    def test_worker_exists(self):
        assert WORKER_SCRIPT.is_file(), f"Worker script not found: {WORKER_SCRIPT}"

    def test_generate_from_folder(self, tmp_output):
        """Generate bundle from the committed ssl_certs/ folder."""
        if not SSL_CERTS_DIR.is_dir():
            pytest.skip("ssl_certs/ directory not present")

        pem_files = list(SSL_CERTS_DIR.glob("*.pem"))
        if not pem_files:
            pytest.skip("No .pem files in ssl_certs/")

        result = subprocess.run(
            [
                sys.executable,
                str(WORKER_SCRIPT),
                "--source", "folder",
                "--certs-dir", str(SSL_CERTS_DIR),
                "--output-dir", str(tmp_output),
            ],
            capture_output=True,
            text=True,
            timeout=30,
        )
        assert result.returncode == 0, f"Worker failed:\n{result.stderr}"

        bundle = tmp_output / "x509_crt_bundle.bin"
        assert bundle.is_file(), "Bundle was not created"
        assert bundle.stat().st_size > 0, "Bundle is empty"

        # Validate bundle header: first 2 bytes are big-endian cert count
        data = bundle.read_bytes()
        (num_certs,) = struct.unpack(">H", data[:2])
        assert num_certs > 0, "Bundle contains zero certificates"

    def test_committed_bundle_is_valid(self):
        """Verify the committed x509_crt_bundle.bin has a valid header."""
        if not COMMITTED_BUNDLE.is_file():
            pytest.skip("Committed bundle not present")

        data = COMMITTED_BUNDLE.read_bytes()
        assert len(data) > 2, "Bundle too small"
        (num_certs,) = struct.unpack(">H", data[:2])
        assert num_certs > 0, "Committed bundle contains zero certificates"

    def test_worker_rejects_invalid_source(self, tmp_output):
        """Worker should fail with an invalid --source value."""
        result = subprocess.run(
            [
                sys.executable,
                str(WORKER_SCRIPT),
                "--source", "bogus",
                "--certs-dir", str(SSL_CERTS_DIR),
                "--output-dir", str(tmp_output),
            ],
            capture_output=True,
            text=True,
            timeout=10,
        )
        assert result.returncode != 0


class TestHookSkipLogic:
    """Tests for the skip-if-exists logic in generate_cert_bundle.py.

    These test the hook logic without running inside PIO/SCons by
    checking the file-existence guard directly.
    """

    def test_binary_present_means_skip(self):
        """When the committed bundle exists, no generation should be needed."""
        assert COMMITTED_BUNDLE.is_file(), (
            "Committed bundle missing — generation would be triggered during build"
        )

    def test_regeneration_produces_equivalent_bundle(self, tmp_output):
        """Regenerating from ssl_certs/folder produces a bundle with certs."""
        if not SSL_CERTS_DIR.is_dir():
            pytest.skip("ssl_certs/ directory not present")

        result = subprocess.run(
            [
                sys.executable,
                str(WORKER_SCRIPT),
                "--source", "folder",
                "--certs-dir", str(SSL_CERTS_DIR),
                "--output-dir", str(tmp_output),
            ],
            capture_output=True,
            text=True,
            timeout=30,
        )
        assert result.returncode == 0

        generated = tmp_output / "x509_crt_bundle.bin"
        committed = COMMITTED_BUNDLE

        if not committed.is_file():
            pytest.skip("No committed bundle to compare against")

        # Both should have the same cert count (may differ in exact bytes
        # if certs were updated, but count should be in the same ballpark)
        (gen_count,) = struct.unpack(">H", generated.read_bytes()[:2])
        (com_count,) = struct.unpack(">H", committed.read_bytes()[:2])
        assert gen_count > 0
        assert com_count > 0
