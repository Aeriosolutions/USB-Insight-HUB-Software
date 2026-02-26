#!/usr/bin/env python
#
# PIO pre-script: ensure x509_crt_bundle.bin exists.
#
# If the binary is already present (committed in git), this script does
# nothing and requires no external dependencies.  If missing, it bootstraps
# a project-local venv, installs cryptography + requests, and delegates
# generation to generate_cert_bundle_worker.py.
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess
import sys
import venv
from pathlib import Path

Import("env")

# --- Paths (relative to project dir) ---
project_dir = Path(env.subst("$PROJECT_DIR"))
binary_file = project_dir / "src" / "certs" / "x509_crt_bundle.bin"
scripts_dir = project_dir / "scripts"
venv_dir = scripts_dir / ".venv"
worker_script = scripts_dir / "generate_cert_bundle_worker.py"


def status(msg):
    sys.stderr.write("SSL Cert Store: %s\n" % msg)


def _get_venv_python():
    """Return path to the venv's Python interpreter."""
    if sys.platform == "win32":
        return venv_dir / "Scripts" / "python.exe"
    return venv_dir / "bin" / "python"


def _ensure_venv():
    """Create project-local venv and install dependencies if needed."""
    venv_python = _get_venv_python()
    if not venv_python.is_file():
        status("Creating project-local venv at %s ..." % venv_dir)
        venv.create(str(venv_dir), with_pip=True)
    status("Ensuring cryptography and requests are installed ...")
    subprocess.check_call(
        [str(venv_python), "-m", "pip", "install", "--quiet",
         "cryptography", "requests"],
    )
    return venv_python


# --- Main logic ---
if binary_file.is_file():
    status("Bundle exists at %s -- skipping generation" % binary_file)
else:
    status("Bundle not found at %s, generating ..." % binary_file)
    venv_python = _ensure_venv()

    cert_source = env.GetProjectOption("board_ssl_cert_source")
    certs_dir = str(project_dir / "ssl_certs")
    output_dir = str(project_dir / "src" / "certs")

    subprocess.check_call([
        str(venv_python),
        str(worker_script),
        "--source", cert_source,
        "--certs-dir", certs_dir,
        "--output-dir", output_dir,
    ])
    status("Bundle generation complete")
