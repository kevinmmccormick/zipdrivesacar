; Gameplay/state update routines

InitGame:
    subroutine
    lda #STATE_BOOT
    sta game_state
    lda #MODE_CRUISE
    sta game_mode
    lda #$5A
    sta rng_seed
    lda #80
    sta player_x
    lda #0
    sta input_previous
    sta prev_switches
    sta road_scroll
    sta difficulty
    sta state_timer
    sta invuln_timer
    sta hazard_active
    sta snack_active
    sta pit_active
    jsr InitSound
    rts

StartNewGame:
    subroutine
    lda #STATE_PLAY
    sta game_state
    lda #80
    sta player_x
    lda #3
    sta lives
    lda #$00
    sta score_lo_bcd
    sta score_hi_bcd
    sta distance_lo
    sta distance_hi
    sta road_scroll
    sta difficulty
    sta hazard_active
    sta snack_active
    sta pit_active
    sta invuln_timer
    sta player_anim
    sta fire_hold
    sta state_timer
    lda #$00
    sta next_life_lo_bcd
    lda #$05
    sta next_life_hi_bcd
    lda #15
    sta comfort_meter
    lda game_mode
    cmp #MODE_ZEN
    bne .notZen
    lda #MOOD_ZEN
    bne .moodDone
.notZen
    lda #MOOD_CHILL
.moodDone
    sta mood

    ; Initial timers by mode.
    ldx game_mode
    lda ModeSpawnBaseTable,x
    sta spawn_timer
    lda ModeSnackBaseTable,x
    sta snack_timer
    lda ModeComfortTimerTable,x
    sta comfort_timer
    lda ModePitTimerTable,x
    sta pit_timer
    rts

RunFrameLogic:
    subroutine
    jsr ReadControls
    jsr UpdateStateMachine
    jsr UpdateSound
    jsr PrepareObjectPointers
    jsr BuildHUD
    rts

UpdateStateMachine:
    subroutine
    lda game_state
    cmp #STATE_TITLE
    beq .title
    cmp #STATE_PLAY
    beq .play
    cmp #STATE_HIT
    beq .hit
    cmp #STATE_GAME_OVER
    beq .gameover
    ; BOOT falls through to title.
    lda #STATE_TITLE
    sta game_state
    rts
.title
    jmp UpdateTitle
.play
    jmp UpdatePlay
.hit
    jmp UpdateHit
.gameover
    jmp UpdateGameOver

ReadControls:
    subroutine
    lda #0
    sta input_current

    lda SWCHA
    sta temp0

    ; Left (bit 6, active low)
    lda temp0
    and #$40
    bne .noLeft
    lda input_current
    ora #INPUT_LEFT
    sta input_current
.noLeft
    ; Right (bit 7, active low)
    lda temp0
    and #$80
    bne .noRight
    lda input_current
    ora #INPUT_RIGHT
    sta input_current
.noRight
    ; Up (bit 4)
    lda temp0
    and #$10
    bne .noUp
    lda input_current
    ora #INPUT_UP
    sta input_current
.noUp
    ; Down (bit 5)
    lda temp0
    and #$20
    bne .noDown
    lda input_current
    ora #INPUT_DOWN
    sta input_current
.noDown

    ; Fire button (INPT4 bit7 low when pressed)
    lda INPT4
    bmi .noFire
    lda input_current
    ora #INPUT_FIRE
    sta input_current
.noFire

    ; Pressed edge = current & (current xor previous)
    lda input_current
    sta temp1
    eor input_previous
    and temp1
    sta input_pressed
    lda temp1
    sta input_previous
    rts

UpdateTitle:
    subroutine
    inc road_scroll

    lda SWCHB
    sta temp0

    ; detect 1->0 edges from previous switches
    lda temp0
    eor #$FF
    and prev_switches
    sta temp1

    ; Select cycles mode.
    lda temp1
    and #SW_SELECT
    beq .checkReset
    inc game_mode
    lda game_mode
    cmp #3
    bcc .selectSfx
    lda #0
    sta game_mode
.selectSfx
    lda #SFX_HORN
    jsr TriggerSFXNoY

.checkReset
    ; Reset starts current mode.
    lda temp0
    and #SW_RESET
    bne .storeSwitches
    jsr StartNewGame

.storeSwitches
    lda temp0
    sta prev_switches
    rts

UpdatePlay:
    subroutine
    inc distance_lo
    bne .distDone
    inc distance_hi
.distDone

    ; difficulty ramps gently over time
    lda distance_hi
    lsr
    lsr
    sta difficulty
    lda game_mode
    cmp #MODE_CHAOS
    bne .diffClamp
    inc difficulty
.diffClamp
    lda difficulty
    cmp #10
    bcc .diffOk
    lda #10
    sta difficulty
.diffOk

    ; invulnerability window after hit
    lda invuln_timer
    beq .invDone
    dec invuln_timer
.invDone

    jsr HandlePlayerInput
    jsr UpdateComfort
    jsr UpdateSpawns
    jsr MoveObjects
    jsr HandleCollisions
    jsr CheckGameOver
    rts

UpdateHit:
    subroutine
    ; flash state, short pause, then return to play or game over
    lda state_timer
    beq .resolve
    dec state_timer
    rts
.resolve
    lda lives
    beq .toGameOver
    lda #STATE_PLAY
    sta game_state
    rts
.toGameOver
    lda #STATE_GAME_OVER
    sta game_state
    rts

UpdateGameOver:
    subroutine
    inc road_scroll
    lda SWCHB
    sta temp0
    lda temp0
    and #SW_RESET
    bne .store
    jsr StartNewGame
.store
    lda temp0
    sta prev_switches
    rts

HandlePlayerInput:
    subroutine
    ; horn/meow: fire edge without direction
    lda input_pressed
    and #INPUT_FIRE
    beq .stepCalc
    lda input_current
    and #(INPUT_LEFT | INPUT_RIGHT)
    bne .stepCalc
    lda #SFX_HORN
    jsr TriggerSFXNoY

.stepCalc
    lda #1
    sta player_dx

    ; drift = fire held + direction
    lda input_current
    and #INPUT_FIRE
    beq .moodPenalty
    lda input_current
    and #(INPUT_LEFT | INPUT_RIGHT)
    beq .moodPenalty
    ldx game_mode
    lda ModeDriftStepTable,x
    sta player_dx

.moodPenalty
    lda mood
    cmp #MOOD_ANNOYED
    bne .boredWobble
    lda player_dx
    cmp #2
    bcc .boredWobble
    dec player_dx

.boredWobble
    lda mood
    cmp #MOOD_BORED
    bne .moveInput
    lda frame_counter
    and #$1F
    bne .moveInput
    lda frame_counter
    and #$20
    beq .boredLeft
    jsr MoveRightByStep
    jmp .moveInput
.boredLeft
    jsr MoveLeftByStep

.moveInput
    lda input_current
    and #(INPUT_LEFT | INPUT_RIGHT)
    beq .inactive
    lda #0
    sta player_anim
    lda input_current
    and #INPUT_LEFT
    beq .checkRight
    jsr MoveLeftByStep
.checkRight
    lda input_current
    and #INPUT_RIGHT
    beq .done
    jsr MoveRightByStep
    jmp .done
.inactive
    inc player_anim
    lda player_anim
    cmp #180
    bcc .done
    lda game_mode
    cmp #MODE_ZEN
    beq .done
    lda #MOOD_BORED
    sta mood
.done
    rts

MoveLeftByStep:
    subroutine
    lda player_x
    sec
    sbc player_dx
    bcs .store
    lda #ROAD_MIN_X
.store
    cmp #ROAD_MIN_X
    bcs .ok
    lda #ROAD_MIN_X
.ok
    sta player_x
    rts

MoveRightByStep:
    subroutine
    lda player_x
    clc
    adc player_dx
    cmp #ROAD_MAX_X
    bcc .store
    lda #ROAD_MAX_X
.store
    sta player_x
    rts

UpdateComfort:
    subroutine
    lda comfort_timer
    beq .drain
    dec comfort_timer
    rts

.drain
    ldx game_mode
    lda ModeComfortTimerTable,x
    sta comfort_timer
    lda mood
    cmp #MOOD_HUNGRY
    bne .drainAmount
    lda comfort_timer
    sec
    sbc #6
    bcs .storeTimer
    lda #8
.storeTimer
    sta comfort_timer

.drainAmount
    lda game_mode
    cmp #MODE_CHAOS
    bne .singleDrain
    jsr DrainComfortOne
.singleDrain
    jsr DrainComfortOne

    lda comfort_meter
    bne .done
    lda game_mode
    cmp #MODE_ZEN
    beq .zenZero
    lda invuln_timer
    bne .done
    lda #MOOD_HUNGRY
    sta mood
    lda #SFX_STARVE
    jsr TriggerSFXNoY
    jsr CauseHit
    rts
.zenZero
    lda #MOOD_ANNOYED
    sta mood
    lda frame_counter
    and #$1F
    bne .done
    lda #SFX_STARVE
    jsr TriggerSFXNoY
.done
    rts

DrainComfortOne:
    subroutine
    lda comfort_meter
    beq .noDrain
    dec comfort_meter
.noDrain
    rts

UpdateSpawns:
    subroutine
    ; Pit stop timer has priority over snack spawning.
    lda pit_timer
    beq .spawnPit
    dec pit_timer
    jmp .hazardTimer

.spawnPit
    lda pit_active
    bne .resetPitTimer
    lda #1
    sta pit_active
    lda #0
    sta pit_y
    sta hazard_active
    jsr NextRandom
    and #$03
    tax
    lda LaneXTable,x
    sta pit_x
.resetPitTimer
    ldx game_mode
    lda ModePitTimerTable,x
    sta pit_timer

.hazardTimer
    lda spawn_timer
    beq .trySpawnHazard
    dec spawn_timer
    jmp .snackTimer

.trySpawnHazard
    ldx game_mode
    lda ModeSpawnBaseTable,x
    sta spawn_timer
    lda spawn_timer
    sec
    sbc difficulty
    bcs .spawnMinOK
    lda #12
.spawnMinOK
    sta spawn_timer
    jsr NextRandom
    and #$1F
    clc
    adc spawn_timer
    sta spawn_timer

    lda hazard_active
    bne .snackTimer
    lda pit_active
    bne .snackTimer
    lda #1
    sta hazard_active
    lda #0
    sta hazard_y
    jsr NextRandom
    and #$03
    sta hazard_type
    tax
    lda LaneXTable,x
    sta hazard_x
    ldx game_mode
    lda ModeHazardSpeedTable,x
    clc
    adc difficulty
    lsr
    clc
    adc ModeHazardSpeedTable,x
    sta hazard_speed

.snackTimer
    lda snack_timer
    beq .trySpawnSnack
    dec snack_timer
    rts

.trySpawnSnack
    ldx game_mode
    lda ModeSnackBaseTable,x
    sta snack_timer
    jsr NextRandom
    and #$3F
    clc
    adc snack_timer
    sta snack_timer

    lda snack_active
    bne .done
    lda pit_active
    bne .done
    lda #1
    sta snack_active
    lda #0
    sta snack_y
    jsr NextRandom
    and #$03
    tax
    lda LaneXTable,x
    sta snack_x
    jsr NextRandom
    and #$03
    cmp #3
    bne .snkTypeOk
    lda #2
.snkTypeOk
    sta snack_type
    ldx game_mode
    lda ModeSnackSpeedTable,x
    sta snack_speed
.done
    rts

MoveObjects:
    subroutine
    inc road_scroll

    lda hazard_active
    beq .snack
    lda hazard_y
    clc
    adc hazard_speed
    sta hazard_y
    cmp #FIELD_LINES
    bcc .snack
    lda #0
    sta hazard_active

.snack
    lda snack_active
    beq .pit
    lda snack_y
    clc
    adc snack_speed
    sta snack_y
    cmp #FIELD_LINES
    bcc .pit
    lda #0
    sta snack_active

.pit
    lda pit_active
    beq .done
    lda pit_y
    clc
    adc #1
    sta pit_y
    cmp #FIELD_LINES
    bcc .done
    lda #0
    sta pit_active
.done
    rts

HandleCollisions:
    subroutine
    lda game_state
    cmp #STATE_PLAY
    beq .playState
    jmp .done

.playState

    ; Hazard hit
    lda hazard_active
    beq .snack
    lda invuln_timer
    bne .snack
    lda pit_active
    bne .snack
    lda hazard_x
    sta temp0
    lda hazard_y
    sta temp1
    jsr CheckPlayerObjectCollision
    bcc .snack
    lda game_mode
    cmp #MODE_ZEN
    beq .zenHazard
    jsr CauseHit
    lda #0
    sta hazard_active
    jmp .snack
.zenHazard
    lda #MOOD_ANNOYED
    sta mood
    lda #SFX_CRASH
    jsr TriggerSFXNoY
    lda #0
    sta hazard_active

.snack
    lda snack_active
    beq .pit
    lda snack_x
    sta temp0
    lda snack_y
    sta temp1
    jsr CheckPlayerObjectCollision
    bcc .pit
    lda snack_type
    cmp #SNACK_BURGER
    beq .burger
    cmp #SNACK_LASAGNA
    beq .lasagna
    lda #$05
    jsr AwardPointsBCD
    lda comfort_meter
    clc
    adc #2
    bne .storeComfort
.burger
    lda #$10
    jsr AwardPointsBCD
    lda comfort_meter
    clc
    adc #4
    bne .storeComfort
.lasagna
    lda #$25
    jsr AwardPointsBCD
    lda comfort_meter
    clc
    adc #6
.storeComfort
    cmp #16
    bcc .comfortOk
    lda #15
.comfortOk
    sta comfort_meter
    lda game_mode
    cmp #MODE_ZEN
    beq .zenMood
    lda #MOOD_CHILL
    bne .moodSet
.zenMood
    lda #MOOD_ZEN
.moodSet
    sta mood
    lda #SFX_SNACK
    jsr TriggerSFXNoY
    lda #0
    sta snack_active

.pit
    lda pit_active
    beq .done
    lda pit_x
    sta temp0
    lda pit_y
    sta temp1
    jsr CheckPlayerObjectCollision
    bcc .done
    lda #$50
    jsr AwardPointsBCD
    lda #15
    sta comfort_meter
    lda game_mode
    cmp #MODE_ZEN
    beq .pitZenMood
    lda #MOOD_CHILL
    bne .pitMoodSet
.pitZenMood
    lda #MOOD_ZEN
.pitMoodSet
    sta mood
    lda #SFX_PIT
    jsr TriggerSFXNoY
    lda #0
    sta pit_active
.done
    rts

CauseHit:
    subroutine
    lda invuln_timer
    bne .done
    lda lives
    beq .toGameOver
    dec lives
    lda #STATE_HIT
    sta game_state
    lda #20
    sta state_timer
    lda #48
    sta invuln_timer
    lda #8
    sta comfort_meter
    lda #MOOD_ANNOYED
    sta mood
    lda #SFX_CRASH
    jsr TriggerSFXNoY
    lda #0
    sta hazard_active
    sta snack_active
    rts
.toGameOver
    lda #STATE_GAME_OVER
    sta game_state
    lda #SFX_CRASH
    jsr TriggerSFXNoY
.done
    rts

CheckGameOver:
    subroutine
    lda lives
    bne .done
    lda game_state
    cmp #STATE_HIT
    beq .done
    lda #STATE_GAME_OVER
    sta game_state
.done
    rts

; temp0 = object x, temp1 = object y
; Carry set on collision
CheckPlayerObjectCollision:
    subroutine
    ; X delta
    lda player_x
    sec
    sbc temp0
    bcs .xPos
    eor #$FF
    clc
    adc #1
.xPos
    cmp #COLLIDE_X_THRESH
    bcs .miss
    ; Y delta against fixed player Y
    lda #PLAYER_SCREEN_Y
    sec
    sbc temp1
    bcs .yPos
    eor #$FF
    clc
    adc #1
.yPos
    cmp #COLLIDE_Y_THRESH
    bcs .miss
    sec
    rts
.miss
    clc
    rts

AwardPointsBCD:
    subroutine
    ; A contains BCD low-byte increment (00..99)
    sta score_add_lo
    lda #$00
    sta score_add_hi
    jsr AddScoreRaw
    lda game_mode
    cmp #MODE_CHAOS
    bne .done
    jsr AddScoreRaw
.done
    rts

AddScoreRaw:
    subroutine
    sed
    clc
    lda score_lo_bcd
    adc score_add_lo
    sta score_lo_bcd
    lda score_hi_bcd
    adc score_add_hi
    sta score_hi_bcd
    cld
    jsr CheckExtraLife
    rts

CheckExtraLife:
    subroutine
.checkLoop
    lda lives
    cmp #9
    bcs .done

    lda score_hi_bcd
    cmp next_life_hi_bcd
    bcc .done
    bne .grant
    lda score_lo_bcd
    cmp next_life_lo_bcd
    bcc .done

.grant
    inc lives
    sed
    clc
    lda next_life_lo_bcd
    adc #$00
    sta next_life_lo_bcd
    lda next_life_hi_bcd
    adc #$05
    sta next_life_hi_bcd
    cld
    jmp .checkLoop
.done
    rts

NextRandom:
    subroutine
    lda rng_seed
    asl
    bcc .noTap
    eor #$1D
.noTap
    eor frame_counter
    sta rng_seed
    rts

PrepareObjectPointers:
    subroutine
    ; Hazard pointer (or pit sprite when pit active)
    lda pit_active
    beq .hazardPtr
    lda #<PitStopSprite
    sta hazard_ptr_lo
    lda #>PitStopSprite
    sta hazard_ptr_hi
    jmp .snackPtr
.hazardPtr
    ldx hazard_type
    lda HazardPtrLo,x
    sta hazard_ptr_lo
    lda HazardPtrHi,x
    sta hazard_ptr_hi

.snackPtr
    ldx snack_type
    lda SnackPtrLo,x
    sta snack_ptr_lo
    lda SnackPtrHi,x
    sta snack_ptr_hi
    rts

BuildHUD:
    subroutine
    lda game_state
    cmp #STATE_TITLE
    beq .title
    cmp #STATE_GAME_OVER
    beq .over
    jmp BuildGameplayHUD
.title
    jmp BuildTitleHUD
.over
    jmp BuildGameOverHUD

BuildGameplayHUD:
    subroutine
    ; Digit offsets (*8): thousands/hundreds/tens/ones.
    lda score_hi_bcd
    lsr
    lsr
    lsr
    lsr
    asl
    asl
    asl
    sta temp0

    lda score_hi_bcd
    and #$0F
    asl
    asl
    asl
    sta temp1

    lda score_lo_bcd
    lsr
    lsr
    lsr
    lsr
    asl
    asl
    asl
    sta temp2

    lda score_lo_bcd
    and #$0F
    asl
    asl
    asl
    sta temp3

    ldy #0
.scoreRowLoop
    ; digit 0
    tya
    clc
    adc temp0
    tax
    lda DigitRows,x
    tax
    lda NibbleReverse,x
    asl
    asl
    asl
    asl
    sta hud_pf0_buf,y

    ; digit 1 -> high nibble of PF1
    tya
    clc
    adc temp1
    tax
    lda DigitRows,x
    asl
    asl
    asl
    asl
    sta score_add_lo

    ; digit 2 -> low nibble of PF1
    tya
    clc
    adc temp2
    tax
    lda DigitRows,x
    ora score_add_lo
    sta hud_pf1_buf,y

    ; digit 3 -> low nibble of PF2 via reverse table
    tya
    clc
    adc temp3
    tax
    lda DigitRows,x
    tax
    lda NibbleReverse,x
    sta hud_pf2_buf,y

    iny
    cpy #8
    bne .scoreRowLoop

    ; lines 8-11: lives pips
    ldx lives
    cpx #10
    bcc .livesIndexOK
    ldx #9
.livesIndexOK
    ldy #8
.livesLoop
    lda LivesPF0Table,x
    sta hud_pf0_buf,y
    lda LivesPF1Table,x
    sta hud_pf1_buf,y
    lda LivesPF2Table,x
    sta hud_pf2_buf,y
    iny
    cpy #12
    bne .livesLoop

    ; lines 12-15: comfort meter bar
    ldx comfort_meter
    cpx #16
    bcc .comfortIndexOK
    ldx #15
.comfortIndexOK
    ldy #12
.comfortLoop
    lda ComfortPF0Table,x
    sta hud_pf0_buf,y
    lda ComfortPF1Table,x
    sta hud_pf1_buf,y
    lda ComfortPF2Table,x
    sta hud_pf2_buf,y
    iny
    cpy #16
    bne .comfortLoop
    rts

BuildTitleHUD:
    subroutine
    ; Mode-specific HUD buffer built from title words.
    ldy #0
.zipLoop
    lda WordZIP_PF0,y
    sta hud_pf0_buf,y
    lda WordZIP_PF1,y
    sta hud_pf1_buf,y
    lda WordZIP_PF2,y
    sta hud_pf2_buf,y
    iny
    cpy #8
    bne .zipLoop

    lda game_mode
    cmp #MODE_CHAOS
    beq .mode2
    cmp #MODE_ZEN
    beq .mode3
    ldx #0
    jsr CopyTitleModeWords
    rts
.mode2
    ldx #1
    jsr CopyTitleModeWords
    rts
.mode3
    ldx #2
    jsr CopyTitleModeWords
    rts

BuildGameOverHUD:
    subroutine
    ldy #0
.overLoop
    lda WordOVER_PF0,y
    sta hud_pf0_buf,y
    lda WordOVER_PF1,y
    sta hud_pf1_buf,y
    lda WordOVER_PF2,y
    sta hud_pf2_buf,y
    iny
    cpy #8
    bne .overLoop

    ; bottom half still shows score digits for context.
    jsr BuildGameplayHUDBottomHalf
    rts

BuildGameplayHUDBottomHalf:
    subroutine
    ; Keep existing score already computed in gameplay builder, so compute and copy
    jsr BuildGameplayHUD
    ; then restore OVER in top 8 rows
    ldy #0
.restoreOver
    lda WordOVER_PF0,y
    sta hud_pf0_buf,y
    lda WordOVER_PF1,y
    sta hud_pf1_buf,y
    lda WordOVER_PF2,y
    sta hud_pf2_buf,y
    iny
    cpy #8
    bne .restoreOver
    rts

; X = mode index (0 cruise, 1 chaos, 2 zen)
CopyTitleModeWords:
    subroutine
    ; lines 8-15 word: GAME1/GAME2/GAME3
    cpx #0
    bne .game2
    ldy #0
.copyGame1
    lda WordGAME1_PF0,y
    sta hud_pf0_buf+8,y
    lda WordGAME1_PF1,y
    sta hud_pf1_buf+8,y
    lda WordGAME1_PF2,y
    sta hud_pf2_buf+8,y
    iny
    cpy #8
    bne .copyGame1
    rts
.game2
    cpx #1
    bne .game3
    ldy #0
.copyGame2
    lda WordGAME2_PF0,y
    sta hud_pf0_buf+8,y
    lda WordGAME2_PF1,y
    sta hud_pf1_buf+8,y
    lda WordGAME2_PF2,y
    sta hud_pf2_buf+8,y
    iny
    cpy #8
    bne .copyGame2
    rts
.game3
    ldy #0
.copyGame3
    lda WordGAME3_PF0,y
    sta hud_pf0_buf+8,y
    lda WordGAME3_PF1,y
    sta hud_pf1_buf+8,y
    lda WordGAME3_PF2,y
    sta hud_pf2_buf+8,y
    iny
    cpy #8
    bne .copyGame3
    rts

TriggerSFXNoY:
    subroutine
    ; convenience wrapper keeping callers simple
    ldy #0
    jmp TriggerSFX

; -----------------------------
; Mode tuning tables
; -----------------------------
ModeSpawnBaseTable:
    .byte 50, 30, 76
ModeSnackBaseTable:
    .byte 92, 150, 70
ModeComfortTimerTable:
    .byte 52, 24, 116
ModePitTimerTable:
    .byte 210, 165, 195
ModeHazardSpeedTable:
    .byte 1, 2, 1
ModeSnackSpeedTable:
    .byte 1, 2, 1
ModeDriftStepTable:
    .byte 2, 3, 2
