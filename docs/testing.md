# Testing and release readiness

Run PowerShell scripts from the repository root. DASM and Stella must be on PATH
or passed as `-DasmExe` and `-StellaExe`. The emulator scripts require Windows;
the image checks use System.Drawing. Tested local tools: DASM 2.20.14.1 and the
Stella 7.0c installation. GNU Make is an alternative build path using a POSIX shell.

```powershell
.\build.ps1
.\smoke-test.ps1
.\self-test.ps1
.\polish-test.ps1
```

| Check | What it establishes | What it does not establish |
| --- | --- | --- |
| Build | Successful assembly and exactly 4096 bytes | Stable video or playability |
| Smoke | Stella remains alive and process CPU time advances | That gameplay is correct |
| Self | Seven saves per mode, state changes and distinct title-state hashes | Correct mode values, movement, score, or collisions |
| Polish | Smoke/self plus snapshots, color/pixel thresholds and distinct mode images | Recognizable art, complete rules, or scanline stability |

Polish scenarios directly set mode/state RAM before capture, so they do not test
the title SELECT path. Save-state hashes include emulator state and can change
even when the intended game operation fails. Color counts can pass on artifacts.
Do not interpret a passing suite as satisfying the original definition of done.

Generated evidence is under `artifacts/` and is not committed. Keep screenshots,
tool versions, ROM SHA-256, and a written record when making a release.

## Manual and behavioral checks still required

- Measure NTSC scanline counts over extended runs in every state and mode,
  including collisions, starvation, pit events, and restart.
- Confirm title text and all three mode labels are readable, SELECT cycles, and
  RESET starts/restarts cleanly; inspect the final score at game over.
- Verify steering bounds, faster drift, horn/meow, sound events and recovery
  after hits; distinguish all hazards and snacks visually.
- Assert +5/+10/+25 snack scores, +50 pit score, Chaos doubling, comfort refills,
  life decrement, invulnerability, and Zen nonlethality against RAM values.
- Exercise BCD carries, score wrap at 9999, the 500-point bonus threshold,
  nine-life cap, and recovery after losing a life at the cap.
- Play long sessions in all modes; verify difficulty and comfort pressure.

## Known source-review concerns

Visual inspection of the October 2026 captures shows broad flat-color regions
and a very small car silhouette in Cruise/Chaos. These pass the pixel checks but
still need renderer investigation and visual review before a polished release.

These are findings to reproduce and fix, not completed regression tests:

- Zen shares the general difficulty ramp even though the design asks for mostly
  flat pacing.
- `CheckExtraLife` stops advancing the threshold at the life cap. Deferred
  thresholds and BCD score/threshold wrap need defined behavior.
- Snack/pit mood code loads `MOOD_CHILL` (zero), then uses `bne` as an intended
  unconditional branch; it falls through to `MOOD_ZEN` in lethal modes.
- Game-over HUD construction overwrites the score rows with OVER while leaving
  lives/comfort below, despite a comment claiming score retention.
- RESET is handled on title and game-over screens, not during active play.
- Missile snacks and pip-style lives are simpler than the design's requested
  recognizable food sprites and cat/paw life icons.

This publication cleanup preserves game assembly behavior. These issues should
be addressed in separate changes with focused emulator assertions.
