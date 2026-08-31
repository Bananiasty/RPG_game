#include  "struct.h"
#include "gamestates.h"


std::unique_ptr<object> exploration::create_world_object(const ObjectSpawnInfo& info)
{
    switch (info.type)
    {
    case ObjectType::Chest:
    {
        int slots = drop_object::rand_drop_slots();
        auto loot = rand_loot(nullptr, slots);

        return std::make_unique<chest>(
            info.position,
            &objects.m_chest,
            slots,
            std::move(loot),
            info.rotation_y
        );
    }

    case ObjectType::DeadBody:
    {
        int slots = drop_object::rand_drop_slots();
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

void exploration::spawn_object(const ObjectSpawnInfo& info)
{
    if (current_floor_id < 0 || current_floor_id >= static_cast<int>(floors.size()))
    {
        return;
    }

    auto obj = create_world_object(info);
    if (obj != nullptr)
    {
        floors[current_floor_id].world_objects.push_back(std::move(obj));
    }
}


Vector3 ObjectSpawnInfo::get_random_wall_position(Vector2 dungeon_pos, Vector2 room_size, float& out_rotation_y, float tile_size)
{
    float min_x = (dungeon_pos.x * tile_size - 2.0f);
    float min_z = dungeon_pos.y * tile_size;

    int max_tile_x = (int)room_size.x - 1;
    int max_tile_z = (int)room_size.y - 1;

    int wall = GetRandomValue(0, 3);
    int tile_x = 0;
    int tile_z = 0;

    switch (wall)
    {
    case 0: // Ściana Północna -> patrzy na Południe (w dół osi Z)
        tile_x = GetRandomValue(0, max_tile_x);
        tile_z = 0;
        out_rotation_y = 0.0f;
        break;

    case 1: // Ściana Południowa -> patrzy na Północ (w górę osi Z)
        tile_x = GetRandomValue(0, max_tile_x);
        tile_z = max_tile_z;
        out_rotation_y = 180.0f;
        break;

    case 2: // Ściana Zachodnia -> patrzy na Wschód (w prawo osi X)
        tile_x = 0;
        tile_z = GetRandomValue(0, max_tile_z);
        out_rotation_y = 90.0f;
        break;

    case 3: // Ściana Wschodnia -> patrzy na Zachód (w lewo osi X)
        tile_x = max_tile_x;
        tile_z = GetRandomValue(0, max_tile_z);
        out_rotation_y = 270.0f;
        break;
    }

    Vector3 prop_pos = {
        min_x + ((float)tile_x + 0.5f) * tile_size,
        0.05f,
        min_z + ((float)tile_z + 0.5f) * tile_size
    };

    return prop_pos;
}

ObjectSpawnInfo ObjectSpawnInfo::create_random_wall_prop(ObjectType type, Vector2 dungeon_pos, Vector2 room_size)
{
    ObjectSpawnInfo info;
    info.type = type;
    info.position = get_random_wall_position(dungeon_pos, room_size, info.rotation_y);
    return info;
}


ObjectSpawnInfo ObjectSpawnInfo::create_trapdoor(Vector2 dungeon_pos, Vector2 room_size, int target_floor_id, float tile_size)
{
    ObjectSpawnInfo info;
    info.type = ObjectType::Trapdoor;

    float center_x = (dungeon_pos.x + room_size.x * 0.5f) * tile_size;
    float center_z = (dungeon_pos.y + room_size.y * 0.5f) * tile_size;
    info.position = { center_x, 0.05f, center_z };
    info.target_floor_id = target_floor_id;
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
