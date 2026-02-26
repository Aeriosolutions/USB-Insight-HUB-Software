#!/usr/bin/env python
#
# Standalone x509 certificate bundle generator.
#
# Converts PEM and DER certificates to a custom bundle format which stores
# just the subject name and public key to reduce space.
#
# Bundle format: number of certificates; crt 1 subject name length;
# crt 1 public key length; crt 1 subject name; crt 1 public key; crt 2...
#
# Invoked by the PIO hook (generate_cert_bundle.py) inside a project-local
# venv that has cryptography and requests installed.
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import struct
import sys
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
import requests


MOZILLA_CACERT_URL = "https://curl.se/ca/cacert.pem"
ADAFRUIT_CACERT_URL = (
    "https://raw.githubusercontent.com/adafruit/"
    "certificates/main/data/roots.pem"
)

CA_BUNDLE_BIN_FILE = "x509_crt_bundle.bin"


def status(msg):
    sys.stderr.write("SSL Cert Store: %s\n" % msg)


def critical(msg):
    sys.stderr.write("SSL Cert Store: %s\n" % msg)


class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)


class CertificateBundle:
    def __init__(self):
        self.certificates = []
        self.compressed_crts = []

    def add_from_path(self, crts_path):
        found = False
        for file_path in os.listdir(crts_path):
            found |= self.add_from_file(os.path.join(crts_path, file_path))

        if found is False:
            raise InputError("No valid x509 certificates found in %s" % crts_path)

    def add_from_file(self, file_path):
        try:
            if file_path.endswith(".pem"):
                status("Parsing certificates from %s" % file_path)
                with open(file_path, "r", encoding="utf-8") as f:
                    crt_str = f.read()
                    self.add_from_pem(crt_str)
                    return True

            elif file_path.endswith(".der"):
                status("Parsing certificates from %s" % file_path)
                with open(file_path, "rb") as f:
                    crt_str = f.read()
                    self.add_from_der(crt_str)
                    return True

        except ValueError:
            critical("Invalid certificate in %s" % file_path)
            raise InputError("Invalid certificate")

        return False

    def add_from_pem(self, crt_str):
        """A single PEM file may have multiple certificates."""
        crt = ""
        count = 0
        start = False

        for strg in crt_str.splitlines(True):
            if strg == "-----BEGIN CERTIFICATE-----\n" and start is False:
                crt = ""
                start = True
            elif strg == "-----END CERTIFICATE-----\n" and start is True:
                crt += strg + "\n"
                start = False
                self.certificates.append(
                    x509.load_pem_x509_certificate(crt.encode(), default_backend())
                )
                count += 1
            if start is True:
                crt += strg

        if count == 0:
            raise InputError("No certificate found")

        status("Successfully added %d certificates" % count)

    def add_from_der(self, crt_str):
        self.certificates.append(
            x509.load_der_x509_certificate(crt_str, default_backend())
        )
        status("Successfully added 1 certificate")

    def create_bundle(self):
        self.certificates = sorted(
            self.certificates,
            key=lambda cert: cert.subject.public_bytes(default_backend()),
        )

        bundle = struct.pack(">H", len(self.certificates))

        for crt in self.certificates:
            pub_key = crt.public_key()
            pub_key_der = pub_key.public_bytes(
                serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo
            )
            sub_name_der = crt.subject.public_bytes(default_backend())

            name_len = len(sub_name_der)
            key_len = len(pub_key_der)
            len_data = struct.pack(">HH", name_len, key_len)

            bundle += len_data
            bundle += sub_name_der
            bundle += pub_key_der

        return bundle


def download_cacert_file(source, certs_dir):
    if source == "mozilla":
        url = MOZILLA_CACERT_URL
    elif source == "adafruit":
        url = ADAFRUIT_CACERT_URL
    else:
        raise InputError("Invalid certificate source: %s" % source)

    response = requests.get(url)

    if response.status_code == 200:
        os.makedirs(certs_dir, exist_ok=True)
        output_file = os.path.join(certs_dir, "cacert.pem")
        with open(output_file, "w", encoding="utf-8") as f:
            f.write(response.text)
        status("Certificate bundle downloaded to: %s" % output_file)
    else:
        status("Failed to fetch the certificate bundle.")


def main():
    parser = argparse.ArgumentParser(
        description="Generate x509 certificate bundle for ESP32"
    )
    parser.add_argument(
        "--source", required=True, choices=["mozilla", "adafruit", "folder"],
        help="Certificate source"
    )
    parser.add_argument("--certs-dir", required=True, help="Directory with PEM/DER certs")
    parser.add_argument("--output-dir", required=True, help="Output directory for bundle")
    args = parser.parse_args()

    certs_dir = Path(args.certs_dir)
    output_dir = Path(args.output_dir)

    bundle = CertificateBundle()

    if args.source in ("mozilla", "adafruit"):
        download_cacert_file(args.source, str(certs_dir))
        bundle.add_from_file(os.path.join(str(certs_dir), "cacert.pem"))
    elif args.source == "folder":
        bundle.add_from_path(str(certs_dir))

    status("Successfully added %d certificates in total" % len(bundle.certificates))

    crt_bundle = bundle.create_bundle()

    os.makedirs(str(output_dir), exist_ok=True)
    output_file = output_dir / CA_BUNDLE_BIN_FILE

    with open(str(output_file), "wb") as f:
        f.write(crt_bundle)

    status("Successfully created %s" % output_file)


if __name__ == "__main__":
    try:
        main()
    except InputError as e:
        print(e)
        sys.exit(2)
