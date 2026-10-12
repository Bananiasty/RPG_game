#pragma once
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <utility>
#include <cstdint>
#include <cmath>
#include "raylib.h"
#include "world/ObjectSpawnInfo.h"

struct object;
class enemy;

enum class TileType : uint8_t
{
	Empty = 0,
	Wall = 1,
	WalkableProp = 2,
	SolidProp = 3
};

// SIATKA PIETRA
struct floor_data
{
	int floor_id;
	int grid_width = 100;
	int grid_height = 100;
	float tile_size = 2.0f;

	std::vector<TileType> tile_grid;
	std::vector<std::unique_ptr<object>> world_objects;

	void init_grid(int w, int h, float size = 2.0f)
	{
		grid_width = w;
		grid_height = h;
		tile_size = size;
		tile_grid.assign(w * h, TileType::Empty);
	}

	int get_index(int gx, int gz) const
	{
		if (gx < 0 || gx >= grid_width || gz < 0 || gz >= grid_height)
		{
			return -1;
		}
		return gz * grid_width + gx;
	}

	bool is_walkable(int gx, int gz) const
	{
		int idx = get_index(gx, gz);
		if (idx == -1)
		{
			return false;
		}
		return tile_grid[idx] != TileType::Wall && tile_grid[idx] != TileType::SolidProp;
	}

	bool is_spawnable(int gx, int gz) const
	{
		int idx = get_index(gx, gz);
		if (idx == -1)
		{
			return false;
		}
		return tile_grid[idx] == TileType::Empty;
	}

	void set_tile(int gx, int gz, TileType type)
	{
		int idx = get_index(gx, gz);
		if (idx != -1)
		{
			tile_grid[idx] = type;
		}
	}
};

// --- Węzły Mapy ---
struct Node
{
	int left_id = -1;
	int right_id = -1;
	int previous_id = -1;

	bool discovered = false;

	int room_x = 0;
	int room_y = 0;
	int room_width = 5;
	int room_length = 5;

	std::vector<int> enemy_ids;
	std::vector<ObjectSpawnInfo> pending_props;
};

// --- Dungeon Floor ---
struct dungeon_floor
{
	std::map<int, Node> world_map;
	std::vector<std::vector<int>> dungeon;
	std::vector<std::unique_ptr<object>> world_objects;
	std::vector<enemy*> active_enemies;
	bool is_generated = false;

	dungeon_floor() = default;
	dungeon_floor(const dungeon_floor&) = delete;
	dungeon_floor& operator=(const dungeon_floor&) = delete;
	dungeon_floor(dungeon_floor&&) noexcept = default;
	dungeon_floor& operator=(dungeon_floor&&) noexcept = default;

	std::set<std::pair<int, int>> occupied_tiles;
	std::set<std::pair<int, int>> solid_tiles;

	bool is_tile_free(int gx, int gz) const
	{
		return occupied_tiles.find({ gx, gz }) == occupied_tiles.end();
	}

	bool is_tile_solid(int gx, int gz) const
	{
		return solid_tiles.find({ gx, gz }) != solid_tiles.end();
	}

	void occupy_tile(int gx, int gz, bool is_solid = true)
	{
		occupied_tiles.insert({ gx, gz });
		if (is_solid)
		{
			solid_tiles.insert({ gx, gz });
		}
	}

	std::pair<int, int> world_to_grid(Vector3 pos, float tile_size = 2.0f) const
	{
		int gx = static_cast<int>(floorf((pos.x + 2.0f) / tile_size));
		int gz = static_cast<int>(floorf(pos.z / tile_size));
		return { gx, gz };
	}
};

// --- NodeConfig ---
struct NodeConfig
{
	int floor_id = 0;
	int room_id = 0;
	int left = -1;
	int right = -1;
	int previous = -1;
	bool discovered = false;
	Vector2 dungeon_pos = { 0.0f, 0.0f };
	Vector2 room_size = { 0.0f, 0.0f };
	std::vector<int> enemy_ids;
	std::vector<ObjectSpawnInfo> props;
};

// --- Fizyka, Walka i Kończyny ---
struct collisions
{
	bool blokuj_ruch_plus_x = false;
	bool blokuj_ruch_minus_x = false;
	bool blokuj_ruch_plus_z = false;
	bool blokuj_ruch_minus_z = false;
};
