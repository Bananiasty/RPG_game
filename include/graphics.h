#pragma once
#include "entities/Character.h"
#include "gamestates.h"
#include "ui/ItemTooltipUI.h"
#include "ui/InventoryUI.h"
#include "ui/LootUI.h"
#include "ui/HUD.h"
#include "ui/BattleUI.h"
#include "ui/LoginUI.h"
#include "render/DungeonRenderer.h"

constexpr float GAME_WIDTH = 1920.0f;
constexpr float GAME_HEIGHT = 1080.0f;

void graphics_init();

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
