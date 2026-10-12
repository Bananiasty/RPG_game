#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <utility>
#include <cstdint>
#include <cstddef>
#include <string>
#include "raylib.h"
#include "raymath.h"
#include "textureManager.h"

// --- Forward Declarations ---
class exploration;
class item;
class enemy;
class player;
struct object;
struct ObjectSpawnInfo;
class Event;
struct dungeon_floor;

// ---Typy i Enumy ---
#include "combat/BodyPart.h"
#include "combat/Limb.h"

#include "inventory/Inventory.h"
#include "world/ObjectSpawnInfo.h"
#include "world/DungeonTypes.h"
#include "world/WorldObject.h"
#include "world/Pathfinding.h"
#include "entities/EnemyConfig.h"


// --- Render & Efekty ---
struct SpriteRenderParams
{
	Vector3 drawPos;
	float targetWidth;
	float targetHeight;
	float frameWidth;
	float frameHeight;
};

SpriteRenderParams GetGhoulRenderParams(Vector3 entityPos, Texture2D texture);

struct floating_text
{
	Vector3 hit_limb;
	std::string text;
	bool is_crit;
	bool is_bleed;
	float max_lifetime = 1.0f;
	float offsetY;
};