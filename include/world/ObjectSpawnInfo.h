#pragma once
#include <cstdint>
#include "raylib.h"

class enemy;
struct dungeon_floor;

enum class ObjectType
{
	Chest,
	Barrel,
	DeadBody,
	p_drop,
	Trapdoor
};

struct ObjectSpawnInfo
{
	ObjectType type;
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	float rotation_y = 0.0f;
	enemy* linked_enemy = nullptr;
	int target_floor_id = -1;

	bool is_random_wall = false;
	Vector2 room_pos = { 0.0f, 0.0f };
	Vector2 room_size = { 0.0f, 0.0f };

	static bool is_tile_blocking_corridor(int gx, int gz, int wall, const dungeon_floor& floor);
	static Vector3 get_random_wall_position(Vector2 dungeon_pos, Vector2 room_size, float& out_rotation_y, dungeon_floor& current_floor, float tile_size = 2.0f);

	static ObjectSpawnInfo create_random_wall_prop(
		ObjectType type,
		Vector2 dungeon_pos,
		Vector2 room_size
	);

	static ObjectSpawnInfo create_trapdoor(
		Vector2 dungeon_pos,
		Vector2 room_size,
		int target_floor_id,
		float tile_size = 2.0f
	);

	static ObjectSpawnInfo create_dead_body(enemy* e, Vector3 pos);
};
