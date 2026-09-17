#include "iostream"
#include  "struct.h"
#include "gamestates.h"
#include "raymath.h"


std::unique_ptr<object> exploration::create_world_object(const ObjectSpawnInfo& info)
{
    switch (info.type)
    {
    case ObjectType::Chest:
    {
        int slots = loot_object::rand_drop_slots();
        auto loot = rand_loot(nullptr, slots);

        return std::make_unique<chest>(
            info.position,
            &objects.chest,
            slots,
            std::move(loot),
            info.rotation_y
        );
    }

    case ObjectType::Barrel:
    {
        int slots = loot_object::rand_drop_slots();
		auto loot = rand_loot(nullptr, slots);
		return std::make_unique<barrel>(
			info.position,
			&objects.barrel,
			slots,
			std::move(loot),
			info.rotation_y
		);
	}

    case ObjectType::DeadBody:
    {
        int slots = loot_object::rand_drop_slots();
        auto loot = rand_loot(info.linked_enemy, slots);

        auto db = std::make_unique<dead_body>(
            info.linked_enemy,
            info.position,
            nullptr,
            slots,
            std::move(loot),
            info.rotation_y
        );
        return db;
    }

    case ObjectType::Trapdoor:
    {
        const ModelAnimation* anim_ptr = (objects.trapdoor_open_animation != nullptr) ? &objects.trapdoor_open_animation[0] : nullptr;
        int total_frames = (anim_ptr != nullptr) ? anim_ptr->keyframeCount : 0;

        return std::make_unique<trapdoor>(
            info.position,
            &objects.trapdoor,
            info.target_floor_id
        );
    }

    default:
        return nullptr;
    }
}

void exploration::spawn_object(ObjectSpawnInfo info)
{
    if (current_floor_id < 0 || current_floor_id >= static_cast<int>(floors.size()))
    {
        return;
    }

    auto& cur_floor = floors[current_floor_id];

    if (info.is_random_wall)
    {
        info.position = ObjectSpawnInfo::get_random_wall_position(
            info.room_pos,
            info.room_size,
            info.rotation_y,
            cur_floor
        );

        // Odrzucenie nieudanego losowania zamiast stawiania w niepoprawnym miejscu
        if (info.position.x < 0.0f)
        {
            return;
        }
    }
    else if (info.type == ObjectType::Trapdoor)
    {
        int center_gx = (int)info.room_pos.x + (int)(info.room_size.x * 0.5f);
        int center_gz = (int)info.room_pos.y + (int)(info.room_size.y * 0.5f);

        cur_floor.occupy_tile(center_gx, center_gz, false);
    }

    auto obj = create_world_object(info);
    if (obj != nullptr)
    {
        cur_floor.world_objects.push_back(std::move(obj));
    }
}

Vector3 ObjectSpawnInfo::get_random_wall_position(
    Vector2 dungeon_pos,
    Vector2 room_size,
    float& out_rotation_y,
    dungeon_floor& current_floor,
    float tile_size)
{
    float min_x = (dungeon_pos.x * tile_size - 2.0f);
    float min_z = dungeon_pos.y * tile_size;

    int max_tile_x = (int)room_size.x - 1;
    int max_tile_z = (int)room_size.y - 1;

    int attempts = 0;
    int chosen_gx = -1;
    int chosen_gz = -1;
    int chosen_tile_x = 0;
    int chosen_tile_z = 0;
    float chosen_rot = 0.0f;

    bool tile_valid = false;

    do
    {
        int wall = GetRandomValue(0, 3);

        switch (wall)
        {
        case 0: // Północ
            chosen_tile_x = GetRandomValue(0, max_tile_x);
            chosen_tile_z = 0;
            chosen_rot = 0.0f;
            break;
        case 1: // Południe
            chosen_tile_x = GetRandomValue(0, max_tile_x);
            chosen_tile_z = max_tile_z;
            chosen_rot = 180.0f;
            break;
        case 2: // Zachód
            chosen_tile_x = 0;
            chosen_tile_z = GetRandomValue(0, max_tile_z);
            chosen_rot = 90.0f;
            break;
        case 3: // Wschód
            chosen_tile_x = max_tile_x;
            chosen_tile_z = GetRandomValue(0, max_tile_z);
            chosen_rot = 270.0f;
            break;
        }

        chosen_gx = (int)dungeon_pos.x + chosen_tile_x;
        chosen_gz = (int)dungeon_pos.y + chosen_tile_z;

        attempts++;

        tile_valid = current_floor.is_tile_free(chosen_gx, chosen_gz);

    } while (!tile_valid && attempts < 50);

    if (!tile_valid)
    {
        return { -1.0f, -1.0f, -1.0f };
    }

    current_floor.occupy_tile(chosen_gx, chosen_gz, true);
    out_rotation_y = chosen_rot;

    Vector3 prop_pos = {
        min_x + ((float)chosen_tile_x + 0.5f) * tile_size,
        0.05f,
        min_z + ((float)chosen_tile_z + 0.5f) * tile_size
    };

    return prop_pos;
}
ObjectSpawnInfo ObjectSpawnInfo::create_random_wall_prop(ObjectType type, Vector2 dungeon_pos, Vector2 room_size)
{
    ObjectSpawnInfo info;
    info.type = type;
    info.is_random_wall = true;
    info.room_pos = dungeon_pos;
    info.room_size = room_size;
    return info;
}

ObjectSpawnInfo ObjectSpawnInfo::create_trapdoor(Vector2 dungeon_pos, Vector2 room_size, int target_floor_id, float tile_size)
{
    ObjectSpawnInfo info;
    info.type = ObjectType::Trapdoor;
    info.room_pos = dungeon_pos;
    info.room_size = room_size;
    info.target_floor_id = target_floor_id;

    float center_x = (dungeon_pos.x + room_size.x * 0.5f) * tile_size;
    float center_z = (dungeon_pos.y + room_size.y * 0.5f) * tile_size;
    info.position = { center_x, 0.05f, center_z };

    return info;
}

ObjectSpawnInfo ObjectSpawnInfo::create_dead_body(enemy* e, Vector3 pos)
{
    ObjectSpawnInfo info;
    info.type = ObjectType::DeadBody;
    info.linked_enemy = e;
    info.position = pos;
    info.rotation_y = 0.0f;
    return info;
}

bool ObjectSpawnInfo::is_tile_blocking_corridor(int gx, int gz, int wall, const dungeon_floor& floor)
{
    if (floor.dungeon.empty())
    {
        return false;
    }

    int grid_w = static_cast<int>(floor.dungeon.size());
    int grid_h = static_cast<int>(floor.dungeon[0].size());

    int check_x = gx;
    int check_z = gz;

    // Sprawdzamy kafelek bezpośrednio za ścianą, przy której stawiamy obiekt
    switch (wall)
    {
    case 0: // Północ
        check_z -= 1;
        break;
    case 1: // Południe
        check_z += 1;
        break;
    case 2: // Zachód
        check_x -= 1;
        break;
    case 3: // Wschód
        check_x += 1;
        break;
    default:
        break;
    }

    // Sprawdzenie, czy pole docelowe mieści się w granicach mapy
    if (check_x >= 0 && check_x < grid_w && check_z >= 0 && check_z < grid_h)
    {
        if (floor.dungeon[check_x][check_z] == 1)
        {
            return true;
        }
    }

    return false;
}

void loot_object::interact(exploration& game, player& player_character)
{
    if (!drop_loot.empty())
    {
        game.active_ui_event = new loot_event(&game, player_character, this);
    }
}

void trapdoor::interact(exploration& game, player& player_character)
{
    if (!is_open)
    {
        is_open = true;
    }
    else if (open_angle >= 75.0f && target_floor_id != -1)
    {
        game.change_floor(target_floor_id);
    }
}

void world_item::interact(exploration& game, player& player_character)
{
    player_character.bag->add_item(this->stored_item.get());
    this->is_destroyed = true;
}