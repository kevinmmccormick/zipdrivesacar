# AGENTS.md
# Zip Drives a Car - Atari 2600 Implementation Guide

## Mission

Build a fully functioning **Atari 2600 NTSC game** called **Zip Drives a Car**.

The result must be a real ROM that:
- assembles successfully
- runs in Stella
- has stable video timing
- is actually playable
- includes the full core feature set from the game concept and instruction manual

This is not a joke prompt anymore. Treat it as a constrained retro game engineering task.

---

## Product summary

**Zip Drives a Car** is a one-player Atari 2600 driving/action game where the player controls a tuxedo cat cruising in a convertible. The player steers left and right, avoids hazards, collects snacks, hits pit stops, manages a simple comfort meter, and tries to survive for a high score.

There are 3 modes:
1. **Cruise Mode**
2. **Chaos Mode**
3. **Zen Meow Mode**

The game should feel like a plausible commercial VCS cartridge from the early 1980s, with a humorous theme but serious implementation quality.

---

## Non-negotiable requirements

The game is only considered complete if all of the following exist and work:

- title screen
- mode selection via console switch
- gameplay loop
- steering
- hazard spawning
- snack spawning
- collisions
- scoring
- lives
- pit stop mechanic
- comfort/meter system
- mode differences across all 3 modes
- horn/meow button action
- drift mechanic
- game over flow
- reset/restart flow

Do not silently omit systems because they are awkward on the 2600.

---

## Platform assumptions

- Platform: Atari 2600 / VCS
- Region: **NTSC**
- CPU: 6507
- RAM: 128 bytes
- Graphics/sound: TIA
- Controller: joystick in left port
- Preferred ROM size: **4K**
- Allowed fallback: **8K F8 bankswitching** if necessary

Start with a 4K design. Only move to 8K if required by actual implementation constraints.

---

## Primary build target

Produce source code compatible with **DASM**.

Expected outputs:
- source files
- build instructions
- assembled ROM binary

Expected build command should be simple and explicit, for example:
`dasm main.asm -f3 -omain.bin`

---

## Definition of done

The project is done only when all of these are true:

### Build
- source assembles without errors
- ROM launches in Stella
- no missing includes or unresolved symbols

### Video
- stable NTSC frame
- no uncontrolled rolling or collapsing
- no obvious kernel instability

### Menu
- title screen appears on boot
- Select cycles all 3 modes
- Reset starts the highlighted mode

### Gameplay
- player can steer reliably
- button causes horn/meow action
- fire + direction causes drift behavior
- hazards appear and move
- snacks appear and can be collected
- pit stops occur periodically
- score increases correctly
- lives decrement correctly in Game 1 and Game 2
- Zen Meow behaves differently and is nonlethal
- game over occurs correctly
- reset can restart cleanly

### Theme fidelity
- hazards are recognizably distinct
- snacks are recognizably distinct
- cat/car identity is visible enough to sell the concept
- the game feels like “Zip Drives a Car,” not a generic block dodger

---

## Recommended engineering philosophy

Prefer:
- simple, robust kernel
- conservative timing
- readable gameplay
- compact data structures
- believable Atari-era abstraction

Avoid:
- overengineered sprite multiplexing
- fragile timing tricks unless necessary
- pretending the hardware can do more than it can
- spending too much ROM on decorative title art

A clean, complete game is better than a flashy broken one.

---

## Core gameplay specification

### Player fantasy
The player is a tuxedo cat named Zip driving a convertible down an endlessly scrolling road.

### Core loop
Each frame:
1. read input
2. update player movement
3. advance road scroll
4. update object timers and spawn logic
5. move hazards/snacks/pit object
6. detect collisions and pickups
7. update score/lives/meter/mood
8. render frame
9. update sound

### Camera model
The car stays near the lower portion of the screen while objects move downward toward it to simulate forward movement.

---

## Game states

Implement these explicit states:

- `STATE_BOOT`
- `STATE_TITLE`
- `STATE_PLAY`
- `STATE_HIT`
- `STATE_GAME_OVER`

Optional:
- `STATE_MODE_SWITCH_DELAY`
- `STATE_START_TRANSITION`

Keep state machine simple and explicit.

---

## Game modes

### GAME 1 - Cruise Mode
Baseline mode.
- moderate object density
- normal speed
- standard scoring
- normal comfort drain
- 3 starting lives

### GAME 2 - Chaos Mode
Harder, sillier mode.
- more hazards
- faster movement
- fewer snacks
- faster comfort drain
- higher scoring pace or multiplier
- 3 starting lives

### GAME 3 - Zen Meow Mode
Toy / relaxing mode.
- no lethal collisions
- very low pressure
- slower or calmer pacing
- player can still collect snacks and pit bonuses
- should still be a valid game state, not just disabled gameplay

---

## Controls

### Joystick
Required:
- Left = move left
- Right = move right

Optional:
- Up = temporary speed increase or ignored
- Down = temporary slowdown or ignored

### Fire button
Required:
- tap with no direction = horn/meow sound
- hold with left/right = drift modifier

### Console switches
Required:
- Select = cycle game modes on title screen
- Reset = start current mode / restart after game over

Color/BW may be ignored safely unless used for an optional feature.

---

## Required systems

## 1. Player movement
Player should move horizontally within road bounds.

Requirements:
- responsive feel
- no excessive inertia
- drift should move faster or wider than standard turn
- movement should not feel broken under mood penalties

Recommended:
- use pixel position with fixed step movement
- optionally quantize collision into coarse lanes

---

## 2. Hazards
Need at least 4 distinct hazard types:

- traffic cone
- vacuum cleaner
- goose
- open manhole

They may share common logic, but their graphics must differ enough to be identifiable.

### Hazard rules
- hazards spawn near top of playfield
- move downward toward player
- collision in Game 1/2 costs one life
- collision in Game 3 is nonlethal
- hazard density depends on mode and difficulty ramp

Minimum acceptable implementation:
- one active hazard at a time

Better implementation:
- one hazard plus one snack active concurrently

Do not exceed what the kernel can handle reliably.

---

## 3. Snacks
Need at least 3 collectible snack types:

- fish bone = 5 points
- burger = 10 points
- lasagna = 25 points

Each should have distinct sprite data.

### Snack rules
- spawn less often than hazards
- move downward
- disappear if missed
- pickup grants score
- pickup restores comfort meter
- pickup may improve mood

Minimum acceptable implementation:
- one active snack at a time

---

## 4. Pit stop
Pit stop is mandatory.

### Pit stop behavior
- appears periodically based on timer or distance
- is visibly distinct from normal snack/hazard
- touching it restores comfort meter significantly or fully
- awards bonus score

Recommended score:
- pit stop = 50 points

Pit stop may temporarily replace normal object spawning during its event window.

---

## 5. Lives
Required:
- 3 starting lives in Cruise and Chaos
- visible life indicator
- life cap at 9

Extra life rule:
- grant one extra life every 500 points
- must not repeatedly trigger from same threshold

Track next bonus threshold explicitly.

---

## 6. Score
Required:
- visible on screen
- at least 4 decimal digits
- BCD strongly recommended

Score sources:
- fish bone = +5
- burger = +10
- lasagna = +25
- pit stop = +50
- survival bonus optional but recommended

---

## 7. Comfort meter
This replaces the joke-ish “food/litter/fatigue” ideas with one practical system.

Implement a meter from 0 to 15.

### Comfort drain
- drains gradually over time
- drains faster in Chaos
- drains slowly or not at all in Zen Meow

### Refill sources
- snacks refill partially
- pit stop refills strongly or fully

### At zero
Game 1 / 2:
- after a brief grace period or immediately, cost one life
- reset meter to partial value after penalty

Game 3:
- no death
- apply mood penalty or sound cue only

This system is mandatory because it gives pit stops a real function.

---

## 8. Mood system
Mood must exist, but keep it lightweight.

Recommended values:
- `MOOD_CHILL`
- `MOOD_ANNOYED`
- `MOOD_HUNGRY`
- `MOOD_BORED`

Optional:
- `MOOD_ZEN`

### Mood causes
- snack pickup improves mood
- collisions worsen mood
- comfort starvation worsens mood
- long inactivity may increase boredom

### Mood effects
Keep effects subtle. Good options:
- slight steering reduction when annoyed
- extra meow sound frequency when annoyed
- slightly faster comfort drain when hungry
- tiny movement wobble when bored

Do not make the game feel unfair or broken.

---

## 9. Difficulty ramp
Need a gentle progression.

Ramp knobs:
- hazard frequency
- hazard speed
- snack rarity
- road curvature amount if implemented
- comfort drain timing

Recommended:
- Cruise ramps slowly
- Chaos ramps faster
- Zen mostly flat

---

## Rendering strategy

## HUD
Top section should show:
- score
- lives
- optional comfort meter

A minimal readable HUD is better than a crowded one.

## Main field
Display:
- road
- player car
- active hazard
- active snack or pit stop

## Road
Use playfield graphics to make a readable road.

Recommended:
- central road with edge markers
- scrolling lane markers achieved by phase-changing playfield pattern
- optional gentle curves by shifting road center a few pixels over time

Road clarity matters more than realism.

---

## Sprite/object allocation guidance

Suggested mapping:
- `P0` = player car
- `P1` = hazard or special active object
- `M0` = snack
- `M1` = optional second small object / effect / HUD use
- `BL` = optional meter segment or pit marker support

This is guidance, not law. Reassign as needed if the kernel is more stable another way.

---

## Art requirements

### Player car
Must read as:
- convertible
- occupant implied as cat
- not just a random square

Do not chase highly detailed art. Strong silhouette is enough.

### Hazard art
Need distinct sprite patterns for:
- cone
- vacuum
- goose
- manhole

### Snack art
Need distinct sprite patterns for:
- fish bone
- burger
- lasagna

### HUD icons
Need:
- life icon, preferably paw or cat head abstraction
- meter representation, preferably simple blocks or pips

### Title text
Need readable title:
- ZIP DRIVES A CAR

Need current mode label:
- GAME 1: CRUISE
- GAME 2: CHAOS
- GAME 3: ZEN MEOW

Crude block text is acceptable.

---

## Sound requirements

Required sounds:
- gameplay engine-ish tone or pulse
- horn/meow press sound
- snack pickup sound
- crash sound
- pit stop/refill sound

Optional:
- title chirp
- game over sting

Keep audio simple and short. Prioritize distinct feedback over musical ambition.

---

## Collision model

Preferred: coarse software collision.

Use:
- x threshold
- y threshold
- active flag checks

TIA collision registers may assist, but do not rely entirely on them if software checks are easier to control.

Collision outcomes:
- hazard hit
- snack pickup
- pit stop pickup

Need short invulnerability window after hit in lethal modes.

---

## Randomness

Implement a lightweight pseudo-random generator.

Acceptable:
- LFSR
- frame counter xor timer
- timer-derived seed plus bit-mixing

RNG must affect:
- object type
- spawn timing variation
- spawn lane/x selection

Spawns should feel varied enough to avoid obvious repetition.

---

## Suggested RAM usage

Use something close to the following. Exact layout may vary.

```text
$80 game_state
$81 game_mode
$82 frame_counter
$83 rng_seed

$84 player_x
$85 player_dx_or_drift
$86 player_anim
$87 lives

$88 score_lo_bcd
$89 score_hi_bcd
$8A next_life_lo_bcd
$8B next_life_hi_bcd

$8C comfort_meter
$8D mood
$8E distance_lo
$8F distance_hi

$90 hazard_active
$91 hazard_type
$92 hazard_x
$93 hazard_y
$94 hazard_speed

$95 snack_active
$96 snack_type
$97 snack_x
$98 snack_y
$99 snack_speed

$9A pit_active
$9B pit_x
$9C pit_y
$9D pit_timer

$9E invuln_timer
$9F sfx_timer

$A0 road_scroll_phase
$A1 road_curve
$A2 spawn_timer
$A3 snack_timer
$A4 comfort_timer
$A5 input_current
$A6 input_pressed
$A7 temp0
$A8 temp1
```

Be disciplined. RAM is not decorative.

## File responsibilities

Suggested file structure:

```text
zipdrives/
  README.md
  Makefile
  main.asm
  constants.inc
  zeropage.inc
  tia.inc
  gfx.inc
  kernel.inc
  game.inc
  sound.inc
  zipdrivesacar.bin
```

### README.md

Must include:

* game description
* controls
* build steps
* emulator instructions
* ROM type (4K or 8K F8)
* known limitations if any

### main.asm

Responsible for:

* vectors
* reset/init
* top-level state dispatch
* file includes

### constants.inc

Responsible for:

* enums
* mode IDs
* state IDs
* tuning constants

### zeropage.inc

Responsible for:

* RAM layout
* variable declarations

### gfx.inc

Responsible for:

* sprite bitmap data
* text glyphs
* playfield patterns

### kernel.inc

Responsible for:

* title render kernel
* gameplay render kernel
* HUD drawing
* scanline timing correctness

### game.inc

Responsible for:

* input
* movement
* spawn logic
* collisions
* scoring
* lives
* mode behavior
* meter/mood logic

### sound.inc

Responsible for:

* SFX routines
* engine tone update
* button/hit/pickup tones

A single-file implementation is acceptable if necessary, but modular separation is preferred.

## Implementation order

Follow this order unless there is a compelling reason not to.

### Phase 1 - Skeleton
- boot/reset
- title screen
- mode selection
- stable kernel
- player car display
- steering

### Phase 2 - Core play
- one hazard
- one snack
- score
- lives
- collision logic
- game over

### Phase 3 - Full feature set
- all three modes
- pit stop
- comfort meter
- mood system
- sound effects
- extra life logic

### Phase 4 - Content completion
- all 4 hazards
- all 3 snacks
- title polish
- HUD polish
- tuning pass

### Phase 5 - Finalization
- build script
- README
- test in Stella
- bug cleanup

Do not jump to polish before the core loop works.

---

## Testing checklist

Run these checks before declaring success.

### Boot/title
- boots to title every time
- no garbage state on warm reset
- mode cycling works correctly

### Cruise Mode
- hazards spawn
- snacks spawn
- collisions cost lives
- pit stop restores meter
- extra life can be earned

### Chaos Mode
- clearly harder than Cruise
- not unfairly impossible
- comfort pressure is noticeable

### Zen Meow
- collisions are nonlethal
- game remains interactive and not broken
- still supports score/snacks/pit events

### System checks
- score increments correctly in BCD
- extra life threshold works once per threshold
- invulnerability works after hit
- no negative wrap bugs
- no obvious object desync
- no unstable scanline artifacts

---

## Quality bar

The finished game should feel like:

- a real Atari 2600 homebrew
- understandable in seconds
- funny without being sloppy
- constrained by hardware in a believable way
- complete enough that someone could hand a ROM to a friend and say “play this”

It should **not** feel like:

- a placeholder prototype
- a broken kernel experiment
- a joke concept with half the systems missing

---

## Allowed simplifications

These are acceptable simplifications if needed:

- only one active hazard at a time
- only one active snack at a time
- pit stop handled as a special event object
- mood effects are subtle and sparse
- road is abstract and lane-like rather than pseudo-3D
- player car is stylized rather than literal

These are **not acceptable omissions**:

- no pit stop
- no Game 3 behavior change
- no snacks
- no drift
- no score/lives HUD
- no comfort system

---

## Stretch goals

Only do these **after the full required game works**:

- road curvature
- scenery palette changes
- day/night cycle
- owl cameo
- Meowami Beach easter egg
- bonus message for high score
- title screen attract animation refinement

Stretch goals must **not jeopardize completion**.

---

## Final instruction to the coding agent

Build the **smallest robust game that fully satisfies the spec**.

Do **not** optimize for cleverness.
Do **not** optimize for theoretical elegance.

Optimize for:

- correctness
- stability
- completeness
- playability
- charm

A **boringly solid Atari 2600 game** is the correct outcome.
