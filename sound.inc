; Audio routines

InitSound:
    lda #0
    sta AUDV0
    sta AUDV1
    sta AUDC0
    sta AUDC1
    sta AUDF0
    sta AUDF1
    sta sfx_id
    sta sfx_timer
    sta engine_phase
    rts

; A = SFX_* constant
TriggerSFX:
    sta sfx_id
    ldx #8
    cmp #SFX_HORN
    bne TriggerSFX_CheckSnack
    ldx #14
    jmp TriggerSFX_Store

TriggerSFX_CheckSnack:
    cmp #SFX_SNACK
    bne TriggerSFX_CheckCrash
    ldx #10
    jmp TriggerSFX_Store

TriggerSFX_CheckCrash:
    cmp #SFX_CRASH
    bne TriggerSFX_CheckPit
    ldx #18
    jmp TriggerSFX_Store

TriggerSFX_CheckPit:
    cmp #SFX_PIT
    bne TriggerSFX_CheckStarve
    ldx #16
    jmp TriggerSFX_Store

TriggerSFX_CheckStarve:
    cmp #SFX_STARVE
    bne TriggerSFX_Store
    ldx #12

TriggerSFX_Store:
    stx sfx_timer
    rts

UpdateSound:
    lda game_state
    cmp #STATE_PLAY
    beq UpdateSound_EngineOn
    cmp #STATE_HIT
    beq UpdateSound_EngineOn
    lda #0
    sta AUDV1
    jmp UpdateSound_SFX

UpdateSound_EngineOn:
    inc engine_phase
    lda #6
    sta AUDC1
    lda game_mode
    cmp #MODE_ZEN
    bne UpdateSound_EngineNormalVol
    lda #1
    sta AUDV1
    jmp UpdateSound_EngineFreq

UpdateSound_EngineNormalVol:
    lda #3
    sta AUDV1

UpdateSound_EngineFreq:
    lda engine_phase
    and #$0F
    clc
    adc #9
    ldx game_mode
    cpx #MODE_CHAOS
    bne UpdateSound_EngineStore
    sec
    sbc #3
UpdateSound_EngineStore:
    sta AUDF1

UpdateSound_SFX:
    lda sfx_timer
    beq UpdateSound_SFXOff
    dec sfx_timer
    lda sfx_id
    cmp #SFX_HORN
    beq UpdateSound_SFXHorn
    cmp #SFX_SNACK
    beq UpdateSound_SFXSnack
    cmp #SFX_CRASH
    beq UpdateSound_SFXCrash
    cmp #SFX_PIT
    beq UpdateSound_SFXPit
    cmp #SFX_STARVE
    beq UpdateSound_SFXStarve
    jmp UpdateSound_SFXOff

UpdateSound_SFXHorn:
    lda #8
    sta AUDC0
    lda sfx_timer
    and #$07
    eor #$07
    clc
    adc #12
    sta AUDF0
    lda #8
    sta AUDV0
    rts

UpdateSound_SFXSnack:
    lda #10
    sta AUDC0
    lda sfx_timer
    and #$03
    clc
    adc #8
    sta AUDF0
    lda #6
    sta AUDV0
    rts

UpdateSound_SFXCrash:
    lda #8
    sta AUDC0
    lda sfx_timer
    asl
    clc
    adc #2
    sta AUDF0
    lda #10
    sta AUDV0
    rts

UpdateSound_SFXPit:
    lda #12
    sta AUDC0
    lda sfx_timer
    and #$07
    clc
    adc #4
    sta AUDF0
    lda #8
    sta AUDV0
    rts

UpdateSound_SFXStarve:
    lda #6
    sta AUDC0
    lda sfx_timer
    and #$07
    clc
    adc #18
    sta AUDF0
    lda #7
    sta AUDV0
    rts

UpdateSound_SFXOff:
    lda #0
    sta AUDV0
    sta sfx_id
    rts
