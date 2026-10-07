; Video kernel and sprite positioning

DrawVisibleFrame:
    lda game_state
    cmp #STATE_TITLE
    bne DrawVisibleFrame_NotTitle
    jmp DrawTitleVisible

DrawVisibleFrame_NotTitle:
    cmp #STATE_GAME_OVER
    bne DrawVisibleFrame_PlayLike
    jmp DrawGameOverVisible

DrawVisibleFrame_PlayLike:
    jmp DrawGameplayVisible

PositionSprites:
    lda #0
    sta HMP0
    sta HMP1
    sta HMM0

    lda game_state
    cmp #STATE_TITLE
    bne PositionSprites_NotTitle

    lda #0
    ldx #0
    jsr SetObjectX
    lda #0
    ldx #1
    jsr SetObjectX
    lda #0
    ldx #2
    jsr SetObjectX
    jmp PositionSprites_Apply

PositionSprites_NotTitle:
    lda player_x
    ldx #0
    jsr SetObjectX

    lda pit_active
    bne PositionSprites_Pit
    lda hazard_active
    beq PositionSprites_NoHazard
    lda hazard_x
    ldx #1
    jsr SetObjectX
    jmp PositionSprites_Snack

PositionSprites_Pit:
    lda pit_x
    ldx #1
    jsr SetObjectX
    jmp PositionSprites_Snack

PositionSprites_NoHazard:
    lda #0
    ldx #1
    jsr SetObjectX

PositionSprites_Snack:
    lda snack_active
    beq PositionSprites_NoSnack
    lda snack_x
    ldx #2
    jsr SetObjectX
    jmp PositionSprites_Apply

PositionSprites_NoSnack:
    lda #0
    ldx #2
    jsr SetObjectX

PositionSprites_Apply:
    lda #0
    sta WSYNC
    sta HMOVE
    rts

; A = X coordinate (0..159), X = object index (0=P0,1=P1,2=M0)
SetObjectX:
    sta WSYNC
    sec
SetObjectX_DivLoop:
    sbc #15
    bcs SetObjectX_DivLoop
    eor #7
    asl
    asl
    asl
    asl
    sta HMP0,x
    sta RESP0,x
    rts

DrawGameplayVisible:
    lda #0
    sta CTRLPF
    lda #$74
    ldx game_state
    cpx #STATE_HIT
    bne DrawGameplayVisible_StoreBg
    lda frame_counter
    and #$04
    beq DrawGameplayVisible_StoreBg
    lda #$32
DrawGameplayVisible_StoreBg:
    sta COLUBK
    ldx game_mode
    lda ModeRoadColorTable,x
    sta COLUPF
    lda #$3E
    sta COLUP0
    lda #$05
    sta NUSIZ0
    lda pit_active
    beq DrawGameplayVisible_HazardColor
    lda #$1E
    bne DrawGameplayVisible_SetCol1

DrawGameplayVisible_HazardColor:
    ldx hazard_type
    lda HazardColorTable,x

DrawGameplayVisible_SetCol1:
    sta COLUP1

    ldx #0
DrawGameplayVisible_HUDLoop:
    sta WSYNC
    lda hud_pf0_buf,x
    sta PF0
    lda hud_pf1_buf,x
    sta PF1
    lda hud_pf2_buf,x
    sta PF2
    lda #0
    sta GRP0
    sta GRP1
    sta ENAM0
    inx
    cpx #HUD_LINES
    bne DrawGameplayVisible_HUDLoop

    lda road_scroll
    and #$04
    beq DrawGameplayVisible_RoadSolid
    lda #$FB
    bne DrawGameplayVisible_RoadStore
DrawGameplayVisible_RoadSolid:
    lda #$FF
DrawGameplayVisible_RoadStore:
    sta temp0

    lda #$00
    sta PF0
    lda temp0
    sta PF1
    lda #$0F
    sta PF2

    ldx #0
DrawGameplayVisible_FieldLoop:
    sta WSYNC

    ; player sprite (P0)
    txa
    sec
    sbc #PLAYER_SCREEN_Y
    cmp #8
    bcs DrawGameplayVisible_NoPlayer
    tay
    lda PlayerCarSprite,y
    bne DrawGameplayVisible_StorePlayer
DrawGameplayVisible_NoPlayer:
    lda #0
DrawGameplayVisible_StorePlayer:
    sta GRP0

    ; hazard/pit sprite (P1)
    lda #0
    sta GRP1
    lda pit_active
    beq DrawGameplayVisible_CheckHazard
    txa
    sec
    sbc pit_y
    cmp #8
    bcs DrawGameplayVisible_AfterP1
    tay
    lda (hazard_ptr_lo),y
    sta GRP1
    jmp DrawGameplayVisible_AfterP1

DrawGameplayVisible_CheckHazard:
    lda hazard_active
    beq DrawGameplayVisible_AfterP1
    txa
    sec
    sbc hazard_y
    cmp #8
    bcs DrawGameplayVisible_AfterP1
    tay
    lda (hazard_ptr_lo),y
    sta GRP1

DrawGameplayVisible_AfterP1:
    ; snack missile (M0)
    lda #0
    sta ENAM0
    lda snack_active
    beq DrawGameplayVisible_AfterSnack
    txa
    sec
    sbc snack_y
    cmp #8
    bcs DrawGameplayVisible_AfterSnack
    tay
    lda (snack_ptr_lo),y
    sta NUSIZ0
    lda #2
    sta ENAM0

DrawGameplayVisible_AfterSnack:
    inx
    cpx #FIELD_LINES
    bne DrawGameplayVisible_FieldLoop

    lda #0
    sta PF0
    sta PF1
    sta PF2
    sta GRP0
    sta GRP1
    sta ENAM0
    lda #$05
    sta NUSIZ0
    rts

DrawTitleVisible:
    lda #0
    sta CTRLPF
    lda #$74
    sta COLUBK
    lda #$1E
    sta COLUPF
    lda #$CE
    sta COLUP0
    sta COLUP1

    ldx #24
    jsr DrawTitleBlankLines

    SET_POINTER title_pf0_lo, WordZIP_PF0
    SET_POINTER title_pf1_lo, WordZIP_PF1
    SET_POINTER title_pf2_lo, WordZIP_PF2
    jsr DrawTitleWordLines

    ldx #8
    jsr DrawTitleBlankLines

    SET_POINTER title_pf0_lo, WordDRIVES_PF0
    SET_POINTER title_pf1_lo, WordDRIVES_PF1
    SET_POINTER title_pf2_lo, WordDRIVES_PF2
    jsr DrawTitleWordLines

    ldx #8
    jsr DrawTitleBlankLines

    SET_POINTER title_pf0_lo, WordACAR_PF0
    SET_POINTER title_pf1_lo, WordACAR_PF1
    SET_POINTER title_pf2_lo, WordACAR_PF2
    jsr DrawTitleWordLines

    ldx #24
    jsr DrawTitleBlankLines

    lda game_mode
    cmp #MODE_CHAOS
    beq DrawTitleVisible_SetGame2
    cmp #MODE_ZEN
    beq DrawTitleVisible_SetGame3
    SET_POINTER title_pf0_lo, WordGAME1_PF0
    SET_POINTER title_pf1_lo, WordGAME1_PF1
    SET_POINTER title_pf2_lo, WordGAME1_PF2
    jmp DrawTitleVisible_DrawGameWord
DrawTitleVisible_SetGame2:
    SET_POINTER title_pf0_lo, WordGAME2_PF0
    SET_POINTER title_pf1_lo, WordGAME2_PF1
    SET_POINTER title_pf2_lo, WordGAME2_PF2
    jmp DrawTitleVisible_DrawGameWord
DrawTitleVisible_SetGame3:
    SET_POINTER title_pf0_lo, WordGAME3_PF0
    SET_POINTER title_pf1_lo, WordGAME3_PF1
    SET_POINTER title_pf2_lo, WordGAME3_PF2
DrawTitleVisible_DrawGameWord:
    jsr DrawTitleWordLines

    ldx #8
    jsr DrawTitleBlankLines

    lda game_mode
    cmp #MODE_CHAOS
    beq DrawTitleVisible_SetChaos
    cmp #MODE_ZEN
    beq DrawTitleVisible_SetZen
    SET_POINTER title_pf0_lo, WordCRUISE_PF0
    SET_POINTER title_pf1_lo, WordCRUISE_PF1
    SET_POINTER title_pf2_lo, WordCRUISE_PF2
    jmp DrawTitleVisible_DrawPrimaryWord
DrawTitleVisible_SetChaos:
    SET_POINTER title_pf0_lo, WordCHAOS_PF0
    SET_POINTER title_pf1_lo, WordCHAOS_PF1
    SET_POINTER title_pf2_lo, WordCHAOS_PF2
    jmp DrawTitleVisible_DrawPrimaryWord
DrawTitleVisible_SetZen:
    SET_POINTER title_pf0_lo, WordZEN_PF0
    SET_POINTER title_pf1_lo, WordZEN_PF1
    SET_POINTER title_pf2_lo, WordZEN_PF2
DrawTitleVisible_DrawPrimaryWord:
    jsr DrawTitleWordLines

    ldx #8
    jsr DrawTitleBlankLines

    lda game_mode
    cmp #MODE_ZEN
    beq DrawTitleVisible_DrawMeow
    ldx #8
    jsr DrawTitleBlankLines
    jmp DrawTitleVisible_Tail
DrawTitleVisible_DrawMeow:
    SET_POINTER title_pf0_lo, WordMEOW_PF0
    SET_POINTER title_pf1_lo, WordMEOW_PF1
    SET_POINTER title_pf2_lo, WordMEOW_PF2
    jsr DrawTitleWordLines

DrawTitleVisible_Tail:
    ldx #64
    jsr DrawTitleBlankLines

    lda #0
    sta PF0
    sta PF1
    sta PF2
    rts

; X = line count
DrawTitleBlankLines:
    lda #0
    sta PF0
    sta PF1
    sta PF2
    sta GRP0
    sta GRP1
    sta ENAM0
DrawTitleBlankLines_Loop:
    sta WSYNC
    dex
    bne DrawTitleBlankLines_Loop
    rts

DrawTitleWordLines:
    ldy #0
DrawTitleWordLines_Loop:
    sta WSYNC
    lda (title_pf0_lo),y
    sta PF0
    lda (title_pf1_lo),y
    sta PF1
    lda (title_pf2_lo),y
    sta PF2
    lda #0
    sta GRP0
    sta GRP1
    sta ENAM0
    iny
    cpy #8
    bne DrawTitleWordLines_Loop
    rts

DrawGameOverVisible:
    lda #0
    sta CTRLPF
    lda #$32
    sta COLUBK
    lda #$4E
    sta COLUPF
    lda #$3E
    sta COLUP0
    lda #$5E
    sta COLUP1

    ldx #0
DrawGameOverVisible_HUDLoop:
    sta WSYNC
    lda hud_pf0_buf,x
    sta PF0
    lda hud_pf1_buf,x
    sta PF1
    lda hud_pf2_buf,x
    sta PF2
    lda #0
    sta GRP0
    sta GRP1
    sta ENAM0
    inx
    cpx #HUD_LINES
    bne DrawGameOverVisible_HUDLoop

    ldx #0
DrawGameOverVisible_FieldLoop:
    sta WSYNC
    lda #0
    sta PF0
    sta PF1
    sta PF2
    sta GRP0
    sta GRP1
    sta ENAM0

    cpx #64
    bcc DrawGameOverVisible_Next
    cpx #72
    bcs DrawGameOverVisible_Next
    txa
    sec
    sbc #64
    tay
    lda WordOVER_PF0,y
    sta PF0
    lda WordOVER_PF1,y
    sta PF1
    lda WordOVER_PF2,y
    sta PF2

DrawGameOverVisible_Next:
    inx
    cpx #FIELD_LINES
    bne DrawGameOverVisible_FieldLoop

    lda #0
    sta PF0
    sta PF1
    sta PF2
    rts
