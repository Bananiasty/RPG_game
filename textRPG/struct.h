#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include "raylib.h"
#include "textureManager.h"
#include <cstddef>

enum class BodyPart { NONE, TORSO, HEAD, LEFT_ARM, RIGHT_ARM, LEFT_LEG, RIGHT_LEG };
			
class exploration;
class item;
class enemy;
class player; 

struct inventory
{
private:
	int max_slots = 100;

public:
	std::vector<item*>items;
	void add_item(item* new_item);
	void del_item(item* rm_item);
};		






enum class ObjectType
{
	Chest,
	Trapdoor,
	DeadBody
};

struct ObjectSpawnInfo
{
	ObjectType type;
	Vector3 position;
	float rotation_y = 0.0f;
	enemy* linked_enemy = nullptr;
	int target_floor_id = 0;

	static Vector3 get_random_wall_position(Vector2 dungeon_pos, Vector2 room_size, float& out_rotation_y, float tile_size = 2.0f);
	static ObjectSpawnInfo create_random_wall_prop(ObjectType type, Vector2 dungeon_pos, Vector2 room_size);
	static ObjectSpawnInfo create_trapdoor(Vector2 dungeon_pos, Vector2 room_size, int target_floor_id, float tile_size = 2.0f);
	static ObjectSpawnInfo create_dead_body(enemy* e, Vector3 pos);
};

struct object
{
	Vector3 position;
	const Model* model_ptr;
	const ModelAnimation* animation_ptr;
	int animation_frame_count = 0;
	float rotation_y = 0.0f;

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
	virtual void update(float deltaTime) {};
	virtual void interact() {};

};
	struct drop_object : object
	{
		int slots;
		std::vector<std::unique_ptr<item>> drop_loot;

		drop_object(Vector3 pos, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot, float rot_y = 0.0f) : object(pos, model, nullptr , 0, rot_y), slots(slots_count), drop_loot(std::move(loot)) {}

		static int rand_drop_slots();

		virtual ~drop_object() = default;
	};
		struct chest : public drop_object
		{
			bool is_open = false;

			chest(Vector3 pos, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot, float rot_y = 0.0f) : drop_object(pos, model, slots_count, std::move(loot), rot_y) {}

			void draw(const Camera3D& camera) const override
			{
				if (model_ptr != nullptr)
				{
					DrawModelEx(*model_ptr, position, { 0.0f, 1.0f, 0.0f }, rotation_y, { 1.5f, 1.5f, 1.5f }, WHITE);
				}
			}
		};

		struct dead_body : public drop_object
		{
			enemy* enemy_ptr = nullptr;

			dead_body(enemy* e, Vector3 pos, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot, float rot_y = 0.0f) : drop_object(pos, model, slots_count, std::move(loot), rot_y), enemy_ptr(e) {}
			
			void draw(const Camera3D& camera) const override
			{
				DrawCube(position, 1.0f, 0.5f, 1.0f, RED);
				DrawCubeWires(position, 1.0f, 0.5f, 1.0f, MAROON);
			}
		};

		struct trapdoor : object
		{
			int target_floor_id = -1;
			bool is_open = false;
			float open_angle = 0.0f;

			trapdoor(Vector3 pos, const Model* model, int target_floor = -1) : object(pos, model, nullptr, 0), target_floor_id(target_floor) {}

			void interact() override
			{
				is_open = true;
			}

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
		};

	class Event;
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

		int enemy_id = -1;
	};


	struct NodeConfig {
		int floor_id = 0;
		int room_id = 0;
		int left = -1;
		int right = -1;
		int previous = -1;
		bool discovered = false;
		Vector2 dungeon_pos = { 0.0f, 0.0f };
		Vector2 room_size = { 0.0f, 0.0f };
		int enemy_id = -1;
		std::vector<ObjectSpawnInfo> props;
	};

	struct dungeon_floor
	{
		std::map<int, Node> world_map;
		std::vector<std::vector<int>> dungeon;
		std::vector<std::unique_ptr<object>> world_objects;
		std::vector<enemy*> active_enemies;

		dungeon_floor() = default;
		dungeon_floor(const dungeon_floor&) = delete;
		dungeon_floor& operator=(const dungeon_floor&) = delete;
		dungeon_floor(dungeon_floor&&) noexcept = default;
		dungeon_floor& operator=(dungeon_floor&&) noexcept = default;
		

	};


struct collisions
{
	bool blokuj_ruch_plus_x = false;
	bool blokuj_ruch_minus_x = false;
	bool blokuj_ruch_plus_z = false;
	bool blokuj_ruch_minus_z = false;
};


struct limb
{
	int hp = 1;
	int max_hp = 1;
	bool is_intact = true;
	bool can_attack = false;
	int damage = 0;

	bool applies_bleed = false;
	int bleed_dmg = 0;
	int bleed_timer = 0;

	

	limb(int max_val = 1, bool can_atk = false, int dmg = 0): hp(max_val), max_hp(max_val), is_intact(true), can_attack(can_atk), damage(dmg) {}

	bool take_damage(int amount)
	{
		if (!is_intact) return false;

		hp -= amount;
		if (hp <= 0)
		{
			hp = 0;
			is_intact = false;
			return true;
		}
		return false;
	}
};
struct limbs_struct
{
	limb head = 1;
	limb torso = 1;
	limb left_arm = 1;
	limb right_arm = 1;
	limb left_leg = 1;
	limb right_leg = 1;

	limbs_struct() = default;
	limbs_struct(int h, int t, int la, int ra, int ll, int rl) : head(h), torso(t), left_arm(la), right_arm(ra), left_leg(ll), right_leg(rl) {}

	limb& get_limb(BodyPart part)
	{
		switch (part)
		{
		case BodyPart::HEAD:
			return head;
		case BodyPart::TORSO:
			return torso;
		case BodyPart::LEFT_ARM:
			return left_arm;
		case BodyPart::RIGHT_ARM:
			return right_arm;
		case BodyPart::LEFT_LEG:
			return left_leg;
		case BodyPart::RIGHT_LEG:
			return right_leg;
		default:
			return torso;
		}
	}
	int get_total_max_hp() const
	{
		int total = 0;

		if (head.is_intact)      total += head.max_hp;
		if (torso.is_intact)     total += torso.max_hp;
		if (left_arm.is_intact)  total += left_arm.max_hp;
		if (right_arm.is_intact) total += right_arm.max_hp;
		if (left_leg.is_intact)  total += left_leg.max_hp;
		if (right_leg.is_intact) total += right_leg.max_hp;

		return total;
	}

};

struct enemy_config {
	int id = -1;
	std::string name = "";
	int max_health;	
	limbs_struct limbs;
	int armor = 0;
	int damage = 0;
	int block_chance = 0;
	int crit_chance = 0;
	int dodge_chance = 0;
	int reduced_head_damage = 0;
	Texture2D texture = {};
	Vector3 position = { 0.0f, 1.0f, 0.0f };
	float rotation = 0.0f;
	int level = 1;
	std::string description = "";
};

//A* pathfinding
struct GridPosition 
{
	int x;
	int z;
};

struct tile_path
{
	int x, z;
	int gCost;
	int hCost;
	int fCost() const { return gCost + hCost; }
	GridPosition parent;
};

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















