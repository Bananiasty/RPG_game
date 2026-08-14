#pragma once
#include "character.h"
#include "gamestates.h"

constexpr float GAME_WIDTH = 1920.0f;
constexpr float GAME_HEIGHT = 1080.0f;

void draw_login_screen(const std::string& current_name, bool has_error);

void graphics_init();

void draw_game_scene(exploration* exp);

void draw_buttons(exploration* e);

void draw_battle_ui(battle* fight);

void draw_inventory_ui(player& p, inventory_state* inv);

void draw_commentary();

void draw_game_over();

void DrawGlobalAnimation();

void DrawExploration(exploration* exp);

void DrawHUD(exploration* exp);

void draw_map_tile(exploration* exp, int x, int y);

void draw_dungeon_map(exploration* exp, float player_x, float player_y);

bool draw_drop(exploration* exp, chest* current_chest, bool& is_open);

inline Vector2 virtual_mouse_pos = { 0.0f, 0.0f };

inline Vector2 GetVirtualMousePosition()
{
    return virtual_mouse_pos;
}

inline Vector2 GetWorldToVirtualScreen(Vector3 position, Camera3D camera, float gameWidth, float gameHeight)
{
    Vector2 windowPos = GetWorldToScreen(position, camera);
    float scaleX = gameWidth / (float)GetScreenWidth();
    float scaleY = gameHeight / (float)GetScreenHeight();

    return Vector2{ windowPos.x * scaleX, windowPos.y * scaleY };
}
