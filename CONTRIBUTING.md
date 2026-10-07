# Contributing

Read [README.md](README.md), [architecture](docs/architecture.md), and
[testing](docs/testing.md). `AGENTS.md` preserves the original design requirements;
it is a target specification, not evidence that every requirement has passed.

Keep changes small and preserve the 4K NTSC target. Explain gameplay changes,
RAM/ROM costs, and timing implications. For kernel changes, measure scanlines in
Stella across title, gameplay, hit, and game-over states. Build with DASM and
record the version, ROM hash, tests run, and remaining manual checks in the PR.

Do not commit generated ROMs, listings, emulator databases, saves, local tool
paths, credentials, or test output. Add focused behavioral assertions when
fixing game rules; changing save-state hashes alone is not a functional test.

Contributions are under GPL-2.0-or-later. Preserve third-party notices and document
the source and license of any imported code or asset. Do not add commercial ROMs.
