# Zip Drives a Car

A one-player Atari 2600 homebrew about Zip, a tuxedo cat driving a convertible.
Steer around cones, vacuums, geese and manholes; collect snacks and pit stops to
keep your comfort meter filled. Written in 6502 assembly for DASM.

**Target:** NTSC Atari 2600 / 6507, 4K single-bank ROM, left-port joystick.
**Status:** playable development build with outstanding validation and rule
issues; see [testing and known issues](docs/testing.md). This is not a claim that
all requirements in the original design have been completed.

## Play

| Control | Action |
| --- | --- |
| Left / Right | Steer within the road |
| Fire without direction | Horn/meow on press |
| Fire + Left / Right | Faster drift, subject to mood |
| Console SELECT on title | Cycle three modes |
| Console RESET on title / game over | Start / restart |

Use Stella's input settings to map these console and joystick controls.
Up/down have no driving action. To choose another mode during a run, reload the ROM.

| Mode | Behavior |
| --- | --- |
| Game 1: Cruise | Baseline pacing, three lives |
| Game 2: Chaos | Faster hazards, fewer snacks, stronger comfort drain, double point awards |
| Game 3: Zen Meow | Nonlethal hazards and comfort depletion, slower starting pace |

Fish bones award 5 points, burgers 10, lasagna 25, and pit stops 50 before the
Chaos multiplier. Snacks restore comfort; pits refill it to 15. Lethal modes
lose a life on hazards or starvation, followed by a brief recovery period.
The HUD shows four score digits, life pips, and comfort. Extra lives use 500-point
thresholds with a cap of nine; see the documented cap/overflow concerns.

## Build

Install [DASM](https://dasm-assembler.github.io/) and optionally
[Stella](https://stella-emu.github.io/) from their official projects. No emulator,
assembler executable, commercial ROM, or BIOS is bundled.

From the repository root, with DASM on PATH:

```powershell
.\build.ps1
```

Or pass its executable location:

```powershell
.\build.ps1 -DasmExe 'C:\path\to\dasm.exe'
```

The script writes `build/zipdrivesacar.bin`, a listing and symbols, validates the
4,096-byte size, and prints SHA-256. It also works when invoked by full path from
another directory. Linux/macOS/MSYS2 users can run `make` with GNU Make and a
POSIX shell; use `make DASM=/path/to/dasm` to override the executable.

The simplest direct build is:

```text
dasm main.asm -f3 -ozipdrivesacar.bin
```

## Run and verify

```powershell
.\test.ps1 -StellaExe 'C:\path\to\Stella.exe'
```

Or open `build/zipdrivesacar.bin` in Stella directly. Select NTSC and a standard
4K cartridge if manual overrides are needed. There is no bankswitching.

[Testing instructions](docs/testing.md) explain smoke, scripted state, and
snapshot checks, their limits, and the manual checks still required. Test scripts
run from the repository root and accept executable-path parameters. Automation
writes ignored evidence to `artifacts/`.

## Development

- [Architecture and source map](docs/architecture.md)
- [Testing and known issues](docs/testing.md)
- [Contributing](CONTRIBUTING.md)
- [Original design specification](AGENTS.md)
- [Security reporting](SECURITY.md)
- [Publication record](docs/publication.md)

One hazard and one snack may be active concurrently. Pit stops replace hazards.
Snacks use limited missile rendering. Hardware timing, art readability, long-run
scoring and mode balance still need the checks listed in the testing document.

## License

Copyright (c) 2026 Kevin M. McCormick. Licensed under **GPL-2.0-or-later**;
see [COPYRIGHT](COPYRIGHT), [LICENSE](LICENSE), and
[third-party notices](THIRD_PARTY_NOTICES.md). Bundled DASM headers retain their
original credits. Distribute corresponding source and notices alongside ROMs.

This independent homebrew is not affiliated with or endorsed by Atari.
