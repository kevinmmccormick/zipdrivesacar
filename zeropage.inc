; Zero-page / RIOT RAM layout

    seg.u Variables
    org $80

game_state       ds 1
game_mode        ds 1
frame_counter    ds 1
rng_seed         ds 1

player_x         ds 1
player_dx        ds 1
player_anim      ds 1
lives            ds 1

score_lo_bcd     ds 1
score_hi_bcd     ds 1
next_life_lo_bcd ds 1
next_life_hi_bcd ds 1

comfort_meter    ds 1
mood             ds 1
distance_lo      ds 1
distance_hi      ds 1

hazard_active    ds 1
hazard_type      ds 1
hazard_x         ds 1
hazard_y         ds 1
hazard_speed     ds 1

snack_active     ds 1
snack_type       ds 1
snack_x          ds 1
snack_y          ds 1
snack_speed      ds 1

pit_active       ds 1
pit_x            ds 1
pit_y            ds 1
pit_timer        ds 1

invuln_timer     ds 1
sfx_timer        ds 1

road_scroll      ds 1
difficulty       ds 1
spawn_timer      ds 1
snack_timer      ds 1
comfort_timer    ds 1
input_current    ds 1
input_pressed    ds 1
prev_switches    ds 1
fire_hold        ds 1

state_timer      ds 1
sfx_id           ds 1
engine_phase     ds 1
score_add_lo     ds 1
score_add_hi     ds 1

hazard_ptr_lo    ds 1
hazard_ptr_hi    ds 1
snack_ptr_lo     ds 1
snack_ptr_hi     ds 1

input_previous   ds 1
temp0            ds 1
temp1            ds 1
temp2            ds 1
temp3            ds 1
title_pf0_lo     ds 1
title_pf0_hi     ds 1
title_pf1_lo     ds 1
title_pf1_hi     ds 1
title_pf2_lo     ds 1
title_pf2_hi     ds 1

hud_pf0_buf      ds 16
hud_pf1_buf      ds 16
hud_pf2_buf      ds 16
