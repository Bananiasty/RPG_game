#include "world/ObjectSpawnInfo.h"
#include "world/DungeonTypes.h"
#include "entities/Enemy.h"
#include "raymath.h"

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

        bool in_bounds = (!current_floor.dungeon.empty() &&
                          chosen_gx >= 0 && chosen_gx < (int)current_floor.dungeon.size() &&
                          chosen_gz >= 0 && chosen_gz < (int)current_floor.dungeon[0].size());

        tile_valid = in_bounds &&
                     current_floor.is_tile_free(chosen_gx, chosen_gz) &&
                     (current_floor.dungeon[chosen_gx][chosen_gz] == 1);

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

    if (check_x >= 0 && check_x < grid_w && check_z >= 0 && check_z < grid_h)
    {
        if (floor.dungeon[check_x][check_z] == 1)
        {
            return true;
        }
    }

    return false;
}
