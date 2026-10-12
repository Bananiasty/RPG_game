#pragma once
#include "raylib.h"

class exploration;

void DrawExploration(exploration* exp);
void draw_game_scene(exploration* exp);
void DrawGlobalAnimation();
void draw_map_tile(exploration* exp, int x, int y, int start_x, int start_y, int tile);
void draw_dungeon_map(exploration* exp, float player_x, float player_y);
