# Architecture

The cartridge is a single-bank 4K image mapped at `$F000-$FFFF`, with reset and
interrupt vectors at `$FFFA`. DASM targets the 6502 instruction set used by the
6507. There is no bankswitching or cartridge RAM.

| File | Responsibility |
| --- | --- |
| `main.asm` | Reset, initialization, vertical sync, blanking, frame dispatch, vectors |
| `constants.inc` | States, modes, input flags, geometry and collision thresholds |
| `zeropage.inc` | RIOT RAM variables and three 16-byte HUD buffers |
| `tia.inc` | Includes hardware symbols and enables legal-opcode macros |
| `vcs.h`, `macro.h` | Bundled DASM hardware definitions and timing/startup macros |
| `game.inc` | Controls, state machine, movement, spawning, collision, BCD scoring, HUD data |
| `kernel.inc` | Horizontal positioning and title/gameplay/game-over rendering |
| `gfx.inc` | Embedded sprite, text, palette, and lookup data |
| `sound.inc` | Engine pulse and prioritized short sound effects |

The RAM declarations occupy 109 bytes from `$80` through `$EC`. The remaining
19 bytes share RIOT RAM with the stack: nested calls need explicit review.

Each frame runs logic and positioning during timer-driven vertical blank, draws
the visible field, and waits through overscan. Constants budget 16 HUD lines and
176 field lines. Timer comments describe intended NTSC timing; they do not prove
that every kernel path fits its budget. See the remaining timing checks.

States are BOOT, TITLE, PLAY, HIT, and GAME_OVER. SELECT cycles title modes;
RESET starts on the title or after game over. PLAY dispatches movement, comfort,
spawning, object movement, collisions, and game-over checks. HIT pauses updates
briefly before returning to play or game over.

P0 draws the car. P1 draws either a hazard or pit stop; a pit event suppresses
hazards. M0 draws the snack using a limited missile pattern. Software collisions
compare object and car coordinates with fixed thresholds. The car stays near
the bottom while objects descend. The RNG mixes a shift-register update with
the frame counter for types, positions, and timer variation.

Score is two packed-BCD bytes; additions clear decimal mode afterward. An
explicit next-life threshold advances by 500, with a nine-life cap. Overflow
and cap behavior need further tests. Comfort ranges from 0 to 15. Mode tables
control spawn, snack, comfort, pit, movement, and drift parameters. Chaos doubles
point awards; Zen makes hazard collisions nonlethal.
