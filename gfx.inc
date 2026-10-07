; Graphics and lookup data

; -----------------------------
; Core sprites (8 scanlines)
; -----------------------------
PlayerCarSprite:
    ; Cat ears + windshield + car body + wheels (no center "snout" holes).
    .byte %00011000
    .byte %00111100
    .byte %01111110
    .byte %11111111
    .byte %11011011
    .byte %11111111
    .byte %11000011
    .byte %10000001

HazardConeSprite:
    .byte %00011000
    .byte %00111100
    .byte %01111110
    .byte %00011000
    .byte %00011000
    .byte %00111100
    .byte %01111110
    .byte %00000000

HazardVacuumSprite:
    .byte %00111100
    .byte %01111110
    .byte %11100110
    .byte %11111110
    .byte %01111100
    .byte %00011000
    .byte %00111100
    .byte %00000000

HazardGooseSprite:
    .byte %00011000
    .byte %00111100
    .byte %01111110
    .byte %11111111
    .byte %01111110
    .byte %00111100
    .byte %00100100
    .byte %01000010
    .byte %00000000

HazardManholeSprite:
    .byte %00111100
    .byte %01111110
    .byte %11100111
    .byte %11000011
    .byte %11000011
    .byte %11100111
    .byte %01111110
    .byte %00111100

PitStopSprite:
    .byte %00111100
    .byte %01111110
    .byte %11011011
    .byte %11000011
    .byte %11011011
    .byte %11011011
    .byte %01111110
    .byte %00111100

HazardPtrLo:
    .byte <HazardConeSprite, <HazardVacuumSprite, <HazardGooseSprite, <HazardManholeSprite
HazardPtrHi:
    .byte >HazardConeSprite, >HazardVacuumSprite, >HazardGooseSprite, >HazardManholeSprite

; Missile width profile by row (NUSIZ0 width bits in upper nibble)
SnackFishRows:
    .byte $05, $15, $25, $15, $25, $15, $05, $05
SnackBurgerRows:
    .byte $15, $35, $35, $25, $35, $35, $15, $05
SnackLasagnaRows:
    .byte $35, $25, $35, $25, $35, $25, $35, $05

SnackPtrLo:
    .byte <SnackFishRows, <SnackBurgerRows, <SnackLasagnaRows
SnackPtrHi:
    .byte >SnackFishRows, >SnackBurgerRows, >SnackLasagnaRows

; -----------------------------
; Colors and lane data
; -----------------------------
LaneXTable:
    .byte 36, 58, 80, 102, 124

HazardColorTable:
    .byte $2E, $0E, $C6, $06

ModeRoadColorTable:
    .byte $0E, $2A, $AE

; -----------------------------
; HUD digit rows (4-bit rows, bit3 is left-most pixel)
; -----------------------------
DigitRows:
; 0
    .byte %1110, %1010, %1010, %1010, %1010, %1010, %1110, %0000
; 1
    .byte %0100, %1100, %0100, %0100, %0100, %0100, %1110, %0000
; 2
    .byte %1110, %0010, %0010, %1110, %1000, %1000, %1110, %0000
; 3
    .byte %1110, %0010, %0010, %1110, %0010, %0010, %1110, %0000
; 4
    .byte %1010, %1010, %1010, %1110, %0010, %0010, %0010, %0000
; 5
    .byte %1110, %1000, %1000, %1110, %0010, %0010, %1110, %0000
; 6
    .byte %1110, %1000, %1000, %1110, %1010, %1010, %1110, %0000
; 7
    .byte %1110, %0010, %0010, %0010, %0010, %0010, %0010, %0000
; 8
    .byte %1110, %1010, %1010, %1110, %1010, %1010, %1110, %0000
; 9
    .byte %1110, %1010, %1010, %1110, %0010, %0010, %1110, %0000

NibbleReverse:
    .byte $0, $8, $4, $C, $2, $A, $6, $E, $1, $9, $5, $D, $3, $B, $7, $F

LivesPF0Table:
    .byte $00, $10, $30, $70, $F0, $F0, $F0, $F0, $F0, $F0
LivesPF1Table:
    .byte $00, $00, $00, $00, $00, $80, $C0, $E0, $F0, $F8
LivesPF2Table:
    .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

ComfortPF0Table:
    .byte $00, $10, $30, $70, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0
ComfortPF1Table:
    .byte $00, $00, $00, $00, $00, $80, $C0, $E0, $F0, $F8, $FC, $FE, $FF, $FF, $FF, $FF
ComfortPF2Table:
    .byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $01, $03, $07

; -----------------------------
; Title words (PF bytes per row, 8 rows each)
; -----------------------------
WordZIP_PF0:
    .byte $00, $F0, $40, $20, $10, $F0, $00, $00
WordZIP_PF1:
    .byte $00, $F0, $A8, $B0, $A0, $E0, $00, $00
WordZIP_PF2:
    .byte $00, $00, $00, $00, $00, $00, $00, $00

WordDRIVES_PF0:
    .byte $00, $B0, $D0, $D0, $D0, $B0, $00, $00
WordDRIVES_PF1:
    .byte $00, $BD, $55, $95, $55, $7A, $00, $00
WordDRIVES_PF2:
    .byte $00, $37, $09, $3B, $21, $1F, $00, $00

WordACAR_PF0:
    .byte $00, $20, $D0, $F0, $D0, $50, $00, $00
WordACAR_PF1:
    .byte $00, $D6, $2D, $3E, $2D, $ED, $00, $00
WordACAR_PF2:
    .byte $00, $00, $00, $00, $00, $00, $00, $00

WordGAME1_PF0:
    .byte $00, $60, $90, $D0, $D0, $E0, $00, $00
WordGAME1_PF1:
    .byte $00, $AF, $7C, $FE, $6C, $6F, $00, $00
WordGAME1_PF2:
    .byte $00, $02, $03, $02, $02, $07, $00, $00

WordGAME2_PF0:
    .byte $00, $60, $90, $D0, $D0, $E0, $00, $00
WordGAME2_PF1:
    .byte $00, $AF, $7C, $FE, $6C, $6F, $00, $00
WordGAME2_PF2:
    .byte $00, $07, $04, $07, $01, $07, $00, $00

WordGAME3_PF0:
    .byte $00, $60, $90, $D0, $D0, $E0, $00, $00
WordGAME3_PF1:
    .byte $00, $AF, $7C, $FE, $6C, $6F, $00, $00
WordGAME3_PF2:
    .byte $00, $07, $04, $07, $04, $07, $00, $00

WordCRUISE_PF0:
    .byte $00, $E0, $90, $90, $90, $E0, $00, $00
WordCRUISE_PF1:
    .byte $00, $AF, $6A, $AA, $6A, $7F, $00, $00
WordCRUISE_PF2:
    .byte $00, $3E, $09, $1F, $0C, $3B, $00, $00

WordCHAOS_PF0:
    .byte $00, $E0, $90, $90, $90, $E0, $00, $00
WordCHAOS_PF1:
    .byte $00, $57, $6D, $FD, $6D, $6F, $00, $00
WordCHAOS_PF2:
    .byte $00, $06, $01, $07, $04, $03, $00, $00

WordZEN_PF0:
    .byte $00, $F0, $C0, $A0, $90, $F0, $00, $00
WordZEN_PF1:
    .byte $00, $E8, $38, $B8, $38, $E8, $00, $00
WordZEN_PF2:
    .byte $00, $00, $00, $00, $00, $00, $00, $00

WordMEOW_PF0:
    .byte $00, $D0, $F0, $F0, $D0, $D0, $00, $00
WordMEOW_PF1:
    .byte $00, $FD, $2D, $AF, $2F, $FD, $00, $00
WordMEOW_PF2:
    .byte $00, $00, $00, $00, $00, $00, $00, $00

WordOVER_PF0:
    .byte $00, $F0, $D0, $D0, $D0, $70, $00, $00
WordOVER_PF1:
    .byte $00, $7E, $65, $76, $65, $BD, $00, $00
WordOVER_PF2:
    .byte $00, $00, $00, $00, $00, $00, $00, $00
