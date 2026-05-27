# UIH Broker — Phase 1 Smallest Validating Slice

**Status:** Scoping
**Date:** 2026-05-27
**Companion:** [broker spec](./2026-04-28-uih-broker-spec.md) · [broker design](./2026-04-28-uih-broker-design.md)

The goal of this slice: produce working code that validates the wire protocol, hub readiness gating, and trust-tier model end-to-end on macOS — without committing yet to display rendering or Espressif tooling. Reviewers can run it, exercise it, and form an independent opinion on the architecture before the spec hardens.

## In scope

**Broker base runtime** — IPC server (UDS), connection identity via `LOCAL_PEERCRED`, per-device FIFO queue, subscription registry, hub worker as async task, transport manager for USB CDC, lease manager (hub leases only), JSONL audit log.

**Enumeration** — mock enumerator + macOS enumerator (IOKit ctypes, ported from `hub_agent.py`).

**Wire protocol subset** — envelope; methods `hello` / `device.list` / `device.info` / `events.subscribe` / `events.publish` / `events.publish.privileged` / `hub.read` / `hub.write` / `device.checkout` / `device.checkin`; events `device-added` / `device-removed` / `device-changed` / `hub-connected` / `hub-disconnected`. Capabilities recognized in audit, not enforced (v1 local trust).

**SDK** — Tier 2 only: `BrokerClient.connect/hello/subscribe/call/publish/close`.

**Default privileged plugin** — `uih-plugin-detect`: VID/PID match, BOS pairing via `bos_container_id`, populates `uih.*` namespace, distinguishes `running` vs `bootloader` from interface layout.

**CLI** — `uih devices` (one-shot list) and `uih watch` (event stream).

**Tests** — integration suite against mock enumerator + mock C2 (UDS pretending to be UIH), plus end-to-end test on real hardware.

## Out of scope (next slice or later)

- `uih-plugin-display` and `uih-plugin-espressif-native*`
- Tier 1 SDK (`Plugin` base class, `@on` / `@action` decorators)
- Action lifecycle (`action.provide` / `action.invoke`)
- `hub.lock`, activity timer, idempotency considerations
- Binary payload chunked forwarding (no bitmap upload yet)
- Linux and Windows enumerators
- Subscription filter expressions (broker-side filtering)
- v2 capability enforcement, token issuance
- LaunchAgent / systemd / NSSM install scripts
- PyInstaller / Windows packaging

## Acceptance criteria

1. Broker starts; IPC accepts connections within 500 ms of process start.
2. Mock UIH `device-added` → `pending` → mock privileged plugin populates `uih.c2_path` → `hub-connected: ready` fires.
3. Real UIH on macOS: same flow with the IOKit enumerator and real `uih-plugin-detect`. No mocks.
4. `uih devices` lists the UIH with `state: ready`, location path, paired USB2/USB3 halves.
5. `uih watch` streams `device-added` / `hub-connected` / `device-removed` for a real plug/unplug cycle.
6. `device.checkout` on the UIH → broker closes C2 → `hub-disconnected: lease` fires → `esptool --port /dev/cu.usbmodem...` succeeds while leased.
7. `device.checkin` → broker reopens C2 → `hub-connected: lease-released` fires.
8. Audit log records every RPC and every privileged-plugin write to `uih.*`.
9. Integration test suite passes on macOS against both mock and real UIH.

## Effort estimate

~1–2 weeks for one person familiar with Python asyncio and macOS IOKit. Lower bound if the `hub_agent.py` IOKit code ports cleanly into the worker; upper bound if `embedded-bridge` Python bindings need polish.

## Proposed layout

```
uih-broker/
├── pyproject.toml
├── uih_broker/
│   ├── master.py           # broker entry point
│   ├── ipc.py              # UDS server + client identity
│   ├── transport.py        # embedded-bridge over USB CDC
│   ├── registry.py         # device registry + merge rules
│   ├── readiness.py        # pending → ready state machine
│   ├── leases.py           # checkout/checkin
│   ├── worker.py           # hub worker
│   ├── audit.py            # JSONL audit log
│   ├── firmware_verbs.py   # bundled version dictionary
│   └── enumerators/
│       ├── base.py
│       ├── supervisor.py
│       ├── mock.py
│       └── workers/macos.py
├── uih_sdk/client.py       # Tier 2 BrokerClient
├── uih_plugin_detect/main.py
├── uih_cli/main.py         # `uih devices`, `uih watch`
└── tests/integration/
```

## Decisions to make before starting

| Question | Default if undecided |
|----------|----------------------|
| `pyserial-asyncio` vs hand-rolled async serial? | `pyserial-asyncio` |
| `embedded-bridge` Python bindings — ready, or build minimal? | Audit binding state in week 1; build minimal if needed |
| Single repo or split into `uih-broker` / `uih-sdk` / `uih-plugin-detect` / `uih-cli`? | Single repo through Phase 1; split when stable |
| Distribution mechanism for the slice (TestPyPI? GitHub-installable?) | `pip install git+https://...` for early reviewers |

## What this slice deliberately does *not* prove

- Display rendering and the 4.5 s firmware activity timeout interplay (next slice).
- Cross-platform parity — Linux and Windows enumerators are Phase 2/3.
- Plugin author ergonomics — Tier 1 decorators are next slice.
- Performance under sustained streaming (bitmap upload, meter streaming).
- v2 capability enforcement.

Reviewing this slice is sufficient to validate the architectural skeleton; non-validation of the above is by design.
