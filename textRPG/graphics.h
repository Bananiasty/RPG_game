#pragma once
#include "character.h"
#include "gamestates.h"

void draw_login_screen(const std::string& current_name, bool has_error);
void graphics_init();
void draw_game_scene(exploration* exp);
void draw_buttons(exploration* e);
void draw_battle_ui(battle* fight);
void draw_inventory_ui(player& p, inventory_state* inv);
void draw_commentary();
void draw_game_won();
void draw_game_over();
void draw_map(map_state* map, exploration* exp);
void draw_chest_drop(exploration* exp);