    processor 6502

    include "constants.inc"
    include "zeropage.inc"
    include "tia.inc"

    seg Code
    org $F000

Reset:
    CLEAN_START
    jsr InitHardware
    jsr InitGame

MainLoop:
    VERTICAL_SYNC

    ; VBLANK (timer-driven) + per-frame updates.
    lda #2
    sta VBLANK
    lda #44              ; ~37 NTSC scanlines at 64-cycle ticks
    sta TIM64T
    jsr RunFrameLogic
    jsr PositionSprites

WaitVBlank:
    lda INTIM
    bne WaitVBlank
    sta WSYNC

    ; Visible frame.
    lda #0
    sta VBLANK
    jsr DrawVisibleFrame

    ; Overscan period.
    lda #2
    sta VBLANK
    lda #36              ; ~30 NTSC scanlines
    sta TIM64T

WaitOverscan:
    lda INTIM
    bne WaitOverscan

    inc frame_counter
    jmp MainLoop

InitHardware:
    lda #0
    sta VSYNC
    sta VBLANK
    sta PF0
    sta PF1
    sta PF2
    sta GRP0
    sta GRP1
    sta ENAM0
    sta ENAM1
    sta ENABL
    sta HMP0
    sta HMP1
    sta HMM0
    sta HMM1
    sta HMBL
    sta HMCLR
    sta CXCLR
    sta NUSIZ0
    sta NUSIZ1
    sta REFP0
    sta REFP1
    sta VDELP0
    sta VDELP1
    sta VDELBL
    rts

    include "sound.inc"
    include "game.inc"
    include "kernel.inc"
    include "gfx.inc"

    org $FFFA
    .word Reset
    .word Reset
    .word Reset
