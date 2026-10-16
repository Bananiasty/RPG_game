#pragma once
#include <vector>
#include <memory>
#include <utility>
#include "raylib.h"
#include "raymath.h"
#include "inventory/Item.h"

class exploration;
class player;
class enemy;

// --- Obiekty Świata ---
struct object
{
	Vector3 position;
	const Model* model_ptr;
	const ModelAnimation* animation_ptr;
	int animation_frame_count = 0;
	float rotation_y = 0.0f;

	bool is_destroyed = false;

	object(Vector3 pos, const Model* model = nullptr, const ModelAnimation* animation = nullptr, int frame_count = 0, float rot_y = 0.0f)
		: position(pos), model_ptr(model), animation_ptr(animation), animation_frame_count(frame_count), rotation_y(rot_y) {}

	virtual ~object() = default;

	virtual void draw(const Camera3D& camera) const
	{
		if (model_ptr != nullptr)
		{
			DrawModelEx(*model_ptr, position, { 0.0f, 1.0f, 0.0f }, rotation_y, { 1.0f, 1.0f, 1.0f }, WHITE);
		}
	}
	virtual void update(float deltaTime) {}
	virtual void interact(exploration& game, player& player_character) {}

	virtual BoundingBox get_interaction_box() const
	{
		Vector3 half_size = { 0.75f, 1.0f, 0.75f };
		return BoundingBox{
			Vector3Subtract(position, half_size),
			Vector3Add(position, half_size)
		};
	}
};

struct loot_object : object
{
	int slots;
	std::vector<std::unique_ptr<item>> drop_loot;

	loot_object(Vector3 pos, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot, float rot_y = 0.0f)
		: object(pos, model, nullptr, 0, rot_y), slots(slots_count), drop_loot(std::move(loot)) {}

	static int rand_drop_slots();

	virtual ~loot_object() = default;

	void interact(exploration& game, player& player_character) override;
};

struct chest : public loot_object
{
	chest(Vector3 pos, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot, float rot_y = 0.0f)
		: loot_object(pos, model, slots_count, std::move(loot), rot_y) {}

	void draw(const Camera3D& camera) const override
	{
		if (model_ptr != nullptr)
		{
			DrawModelEx(*model_ptr, position, { 0.0f, 1.0f, 0.0f }, rotation_y, { 1.5f, 1.5f, 1.5f }, WHITE);
		}
	}
};

struct dead_body : public loot_object
{
	enemy* enemy_ptr = nullptr;

	dead_body(enemy* e, Vector3 pos, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot, float rot_y = 0.0f)
		: loot_object(pos, model, slots_count, std::move(loot), rot_y), enemy_ptr(e) {}

	void draw(const Camera3D& camera) const override
	{
		DrawCube(position, 0.2f, 0.2f, 0.2f, RED);
		DrawCubeWires(position, 0.2f, 0.2f, 0.2f, MAROON);
	}
};

struct barrel : public loot_object
{
	barrel(Vector3 pos, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot, float rot_y = 0.0f)
		: loot_object(pos, model, slots_count, std::move(loot), rot_y) {}

	void draw(const Camera3D& camera) const override
	{
		if (model_ptr != nullptr)
		{
			DrawModelEx(*model_ptr, position, { 0.0f, 1.0f, 0.0f }, rotation_y, { 1.5f, 1.5f, 1.5f }, WHITE);
		}
	}
};

struct p_drop : public loot_object
{
	p_drop(Vector3 pos, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot, float rot_y = 0.0f)
		: loot_object(pos, model, slots_count, std::move(loot), rot_y) {}

	void draw(const Camera3D& camera) const override
	{
		if (model_ptr != nullptr)
		{
			DrawModelEx(*model_ptr, position, { 0.0f, 1.0f, 0.0f }, rotation_y, { 1.0f, 1.0f, 1.0f }, WHITE);
		}
		else
		{
			DrawCube(position, 0.4f, 0.3f, 0.4f, BROWN);
			DrawCubeWires(position, 0.4f, 0.3f, 0.4f, DARKBROWN);
		}
	}
};

struct trapdoor : object
{
	int target_floor_id = -1;
	bool is_open = false;
	float open_angle = 0.0f;

	trapdoor(Vector3 pos, const Model* model, int target_floor = -1)
		: object(pos, model, nullptr, 0), target_floor_id(target_floor) {}

	void interact(exploration& game, player& player_character) override;

	void update(float dt) override
	{
		if (is_open && open_angle < 75.0f)
		{
			open_angle += dt * 120.0f;
			if (open_angle > 75.0f) open_angle = 75.0f;
		}
	}

	void draw(const Camera3D& camera) const override
	{
		if (model_ptr != nullptr)
		{
			DrawPlane({ position.x - 1.0f, position.y, position.z + 1.0f }, { 2.0f, 2.0f }, BLACK);
			DrawModelEx(*model_ptr, position, { 1.0f, 0.0f, 0.0f }, -open_angle, { 1.0f, 1.0f, 1.0f }, WHITE);
		}
	}

	BoundingBox get_interaction_box() const override
	{
		Vector3 box_half_size = { 1.25f, 0.4f, 1.25f };
		Vector3 box_center = position;
		box_center.y += 0.2f;
		box_center.x -= 0.5f;
		box_center.z += 1.2f;

		return BoundingBox{
			Vector3Subtract(box_center, box_half_size),
			Vector3Add(box_center, box_half_size)
		};
	}
};

struct world_item : object
{
	std::unique_ptr<item> stored_item;
	float size_scale = 1.0f;

	world_item(Vector3 pos, const Model* model, std::unique_ptr<item> it, float scale, float rot_y)
		: object(pos, model, nullptr, 0, rot_y), stored_item(std::move(it)), size_scale(scale) {}

	void draw(const Camera3D& camera) const override
	{
		if (model_ptr != nullptr)
		{
			DrawModelEx(*model_ptr, position, { 0.0f, 1.0f, 0.0f }, rotation_y, { size_scale, size_scale, size_scale }, WHITE);
		}
		else
		{
			DrawCube(position, 0.5f * size_scale, 0.5f * size_scale, 0.5f * size_scale, BLUE);
			DrawCubeWires(position, 0.5f * size_scale, 0.5f * size_scale, 0.5f * size_scale, DARKBLUE);
		}
	}

	void interact(exploration& game, player& player_character) override;

	BoundingBox get_interaction_box() const override
	{
		Vector3 box_half_size = { 0.35f * size_scale, 0.35f * size_scale, 0.35f * size_scale };
		return BoundingBox{
			Vector3Subtract(position, box_half_size),
			Vector3Add(position, box_half_size)
		};
	}
};
