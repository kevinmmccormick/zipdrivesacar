# Publication preparation — 2026-10-07

Intended repository: `kevinmmccormick/zipdrivesacar`, public.
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
- CI workflow and GNU Make path have not yet run on GitHub/Linux.

The checks do not establish cycle-accurate timing, complete gameplay correctness,
or real-console compatibility. No stable-release tag is warranted from this
cleanup alone.

## Environment limits and upload

The original workspace files rejected writes in this session, while new
directories were writable. Therefore this clean repository was prepared under
`publication/` without replacing the originals. The original artifacts remain
available locally. Use this directory as the repository root, not its parent.

GitHub CLI could not reach the configured proxy (`127.0.0.1:9`); no browser was
available. No remote repository, push, release, or CI run was verified.

Once GitHub access is restored, run from this publication repository:

```powershell
gh auth status
gh repo create kevinmmccormick/zipdrivesacar --public --source . --remote origin --push --description 'Zip Drives a Car: a 4K NTSC Atari 2600 homebrew in DASM assembly'
```

If the repository already exists, inspect it before adding its remote and
pushing; do not overwrite unrelated work or force-push. Verify the uploaded
commit and Actions build afterward. Release ROMs with corresponding source and
notices; the CI artifact includes these together.
