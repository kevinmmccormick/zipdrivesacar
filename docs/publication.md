# Publication preparation — 2026-10-07

Published repository: [kevinmmccormick/zipdrivesacar](https://github.com/kevinmmccormick/zipdrivesacar), public.
License selected by the owner: GPL-2.0-or-later.

## Scope

This is a clean publication copy of the existing workspace. Original assembly
and vendor headers are preserved without behavioral changes. Build output,
emulator saves, screenshots, SQLite databases, assembler listings, an empty
session file, and local editor configuration are excluded from Git.

Added: build/run documentation, architecture, test coverage and known issues,
contributor/security guidance, license grant/text and provenance assessment,
ignore/line-ending rules, and a GitHub Actions build workflow. Build wrappers use
PATH or explicit tool parameters and produce isolated `build/` outputs. Automated
Stella launches are hidden, run folders are unique, and visual mode-distinction
checks now affect the test exit code.

## Local validation

- DASM 2.20.14.1 assembles successfully: 4096 bytes.
- SHA-256: `4B5EBC4193FB09AB53E6FCBA13808CD1C5573C3E534C8E9F873A01E51F263FD8`.
- Byte-identical to the original workspace ROM; game logic and graphics unchanged.
- PowerShell syntax parsing: passed for all five scripts.
- Stella 7.0c smoke: three of three runs passed.
- Scripted self-test: Cruise, Chaos, Zen passed; three distinct title hashes.
- Visual thresholds: all three modes passed, five snapshots each; title and
  gameplay hashes distinct for all three modes.
- Human inspection of captured Cruise/Chaos gameplay revealed broad flat-color
  regions and a very small car silhouette. The automated pixel thresholds pass
  despite this weak visual result. Do not call this a polished release.
- Source review found rule and presentation concerns recorded in `testing.md`.
- GitHub Actions and the GNU Make build passed on Linux in
  [run 37650394244](https://github.com/kevinmmccormick/zipdrivesacar/actions/runs/37650394244).
  The workflow packages the development ROM with source and license notices.

The checks do not establish cycle-accurate timing, complete gameplay correctness,
or real-console compatibility. No stable-release tag is warranted from this
cleanup alone.

## Workspace consolidation and upload

The original workspace files initially rejected writes. The owner repaired
inherited Windows permissions, and write access was verified without modifying
file contents. The prepared files have now been consolidated into the workspace
root, where a fresh build reproduced the original ROM hash. Original files,
loose generated outputs, and the temporary publication repository were preserved
under the ignored `artifacts/workspace-before-consolidation-*` directory.

The initial session blocked Git metadata writes and outbound GitHub connections.
Switching to Ask for approval enabled approved commands outside the sandbox.
The prepared history (`2128e03`) was imported into the root repository, the
consolidation was committed, and `main` was pushed to the public repository.
The first GitHub build passed. The workspace root is now the active repository;
the archived preparation copy is retained only as a backup.

No stable release has been published. Release ROMs with corresponding source
and notices; the development CI artifact includes these together.
