#include "gamestates.h"
#include "character.h"
#include <vector>
#include <queue>
#include <map>
#include <cmath>
#include <random>
#include <algorithm>



exploration::exploration(player& p) : bohater(p)
{
    active_ui_event = nullptr;
    current_node_id = 1;
    current_floor_id = 0;
    const int total_floors = 2;
    floors.clear();
    floors.reserve(total_floors);
    for (int i = 0; i < total_floors; ++i)
    {
        floors.emplace_back();
    }
    dlugosc = 200;
    szerokosc = 200;

    for (auto& floor : floors)
    {
        floor.dungeon.assign(szerokosc, std::vector<int>(dlugosc, 2));
    }

    world_map_init();
    generate_floor(current_floor_id);

    player_stats_init(&bohater);


    this->camera = { 0 };
    this->camera.position = Vector3{ bohater.position.x, 1.5f, bohater.position.z };
    this->camera.target = Vector3{ bohater.position.x, 1.5f, bohater.position.z + 5.0f };
    this->camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    this->camera.fovy = 80.0f;
    this->camera.projection = CAMERA_PERSPECTIVE;


}
exploration::~exploration()
{
    if (active_ui_event != nullptr)
    {
        delete active_ui_event;
        active_ui_event = nullptr;
    }

    for (auto& floor : floors)
    {
        floor.world_objects.clear();

        for (enemy* e : floor.active_enemies)
        {
            delete e;
        }
        floor.active_enemies.clear();
    }

    for (enemy* e : enemy_pool)
    {
        delete e;
    }
    enemy_pool.clear();

    for (item* it : item_pool)
    {
        delete it;
    }
    item_pool.clear();
}
void exploration::event_check()
{
    // 1. Sprawdzenie aktywnego UI eventu (np. otwarte okno łupu)
    if (active_ui_event != nullptr)
    {
        loot_event* current_loot = dynamic_cast<loot_event*>(active_ui_event);
        if (current_loot != nullptr && !current_loot->is_active)
        {
            delete active_ui_event;
            active_ui_event = nullptr;
        }
        else
        {
            is_hovering_interactive = false;
            return;
        }
    }

    is_hovering_interactive = false;

    Vector3 player_pos = camera.position;
    auto& current_objects_list = floors[current_floor_id].world_objects;

    Vector2 ray_origin_pos = IsCursorHidden()
        ? Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f }: GetMousePosition();

    Ray ray = GetScreenToWorldRay(ray_origin_pos, camera);

    object* targeted_obj = nullptr;
    float closest_hit_distance = 999999.0f;

    for (const auto& obj_ptr : current_objects_list)
    {
        if (!obj_ptr || obj_ptr->is_destroyed)
        {
            continue;
        }

        if (auto* drop_ptr = dynamic_cast<loot_object*>(obj_ptr.get()))
        {
            if (drop_ptr->drop_loot.empty())
            {
                continue;
            }
        }

        float max_dist_sqr = (dynamic_cast<trapdoor*>(obj_ptr.get()) != nullptr) ? 14.0f : 7.25f;
        if (Vector3DistanceSqr(player_pos, obj_ptr->position) > max_dist_sqr)
        {
            continue;
        }

        BoundingBox box = obj_ptr->get_interaction_box();

        RayCollision collision = GetRayCollisionBox(ray, box);

        if (collision.hit && collision.distance < closest_hit_distance)
        {
            closest_hit_distance = collision.distance;
            targeted_obj = obj_ptr.get();
            hovered_point = collision.point;
            is_hovering_interactive = true;
        }
    }

    if (targeted_obj != nullptr && IsKeyPressed(KEY_E))
    {
        targeted_obj->interact(*this, bohater);
    }

    std::erase_if(current_objects_list, [](const auto& ptr)
        {
            return ptr && ptr->is_destroyed;
        });
}


Vector3 exploration::set_enemy_pos(int enemy_id, int room_id, int floor_id, float wall_margin, float corridor_depth)
{
    if (enemy_id == -1)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    enemy* enemy_ptr = this->get_enemy_by_id(enemy_id);
    if (enemy_ptr == nullptr)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    float enemy_y = enemy_ptr->get_position().y;
    delete enemy_ptr;

    if (floor_id < 0 || floor_id >= static_cast<int>(floors.size()))
    {
        return { 0.0f, enemy_y, 0.0f };
    }

    dungeon_floor& target_floor = floors[floor_id];
    Node* room_ptr = this->get_room_by_id(room_id, floor_id);
    if (room_ptr == nullptr)
    {
        return { 0.0f, enemy_y, 0.0f };
    }

    const float tile_size = 2.0f;
    int rx = room_ptr->room_x;
    int rz = room_ptr->room_y;
    int rw = room_ptr->room_width;
    int rh = room_ptr->room_length;

    float fallback_x = (rx + rw / 2.0f) * tile_size;
    float fallback_z = (rz + rh / 2.0f) * tile_size;
    Vector3 fallback_pos = { fallback_x, enemy_y, fallback_z };

    struct CandidateTile
    {
        int gx;
        int gz;
        bool is_corridor;
    };
    std::vector<CandidateTile> candidates;

    auto is_inside_any_room = [&](int gx, int gz) -> bool {
        for (const auto& [id, node] : target_floor.world_map)
        {
            if (gx >= node.room_x && gx < node.room_x + node.room_width &&
                gz >= node.room_y && gz < node.room_y + node.room_length)
            {
                return true;
            }
        }
        return false;
    };

    //Pole pokoju z marginesem od scian
    int margin_tiles = static_cast<int>(std::round(wall_margin / tile_size));
    if (rw > 2 * margin_tiles && rh > 2 * margin_tiles)
    {
        for (int x = rx + margin_tiles; x <= rx + rw - 1 - margin_tiles; ++x)
        {
            for (int z = rz + margin_tiles; z <= rz + rh - 1 - margin_tiles; ++z)
            {
                if (x >= 0 && x < szerokosc && z >= 0 && z < dlugosc)
                {
                    bool walkable = target_floor.dungeon.empty() ||
                                    target_floor.dungeon[x][z] == 1 ||
                                    target_floor.dungeon[x][z] == 2;
                    if (walkable && !target_floor.is_tile_solid(x, z) && target_floor.is_tile_free(x, z))
                    {
                        candidates.push_back({ x, z, false });
                    }
                }
            }
        }
    }

    //Sasiednie korytarze do glebokosci corridor_depth bez marginesu od scian
    int max_corridor_tiles = static_cast<int>(std::round(corridor_depth / tile_size));
    if (max_corridor_tiles > 0 && !target_floor.dungeon.empty())
    {
        std::map<std::pair<int, int>, int> corridor_visited;
        std::queue<std::pair<int, int>> q;

        // Polnoc
        int nz = rz - 1;
        if (nz >= 0)
        {
            for (int x = rx; x < rx + rw; ++x)
            {
                if (x >= 0 && x < szerokosc && target_floor.dungeon[x][nz] == 1 && !is_inside_any_room(x, nz))
                {
                    if (corridor_visited.find({ x, nz }) == corridor_visited.end())
                    {
                        corridor_visited[{ x, nz }] = 1;
                        q.push({ x, nz });
                    }
                }
            }
        }

        // Poludnie
        int sz = rz + rh;
        if (sz < dlugosc)
        {
            for (int x = rx; x < rx + rw; ++x)
            {
                if (x >= 0 && x < szerokosc && target_floor.dungeon[x][sz] == 1 && !is_inside_any_room(x, sz))
                {
                    if (corridor_visited.find({ x, sz }) == corridor_visited.end())
                    {
                        corridor_visited[{ x, sz }] = 1;
                        q.push({ x, sz });
                    }
                }
            }
        }

        // Zachod
        int wx = rx - 1;
        if (wx >= 0)
        {
            for (int z = rz; z < rz + rh; ++z)
            {
                if (z >= 0 && z < dlugosc && target_floor.dungeon[wx][z] == 1 && !is_inside_any_room(wx, z))
                {
                    if (corridor_visited.find({ wx, z }) == corridor_visited.end())
                    {
                        corridor_visited[{ wx, z }] = 1;
                        q.push({ wx, z });
                    }
                }
            }
        }

        // Wschod
        int ex = rx + rw;
        if (ex < szerokosc)
        {
            for (int z = rz; z < rz + rh; ++z)
            {
                if (z >= 0 && z < dlugosc && target_floor.dungeon[ex][z] == 1 && !is_inside_any_room(ex, z))
                {
                    if (corridor_visited.find({ ex, z }) == corridor_visited.end())
                    {
                        corridor_visited[{ ex, z }] = 1;
                        q.push({ ex, z });
                    }
                }
            }
        }

        // BFS wzdluz korytarzy
        const int dx[4] = { 1, -1, 0, 0 };
        const int dz[4] = { 0, 0, 1, -1 };

        while (!q.empty())
        {
            auto [cx, cz] = q.front();
            q.pop();

            int d = corridor_visited[{ cx, cz }];
            if (d < max_corridor_tiles)
            {
                for (int i = 0; i < 4; ++i)
                {
                    int nx = cx + dx[i];
                    int nz = cz + dz[i];

                    if (nx >= 0 && nx < szerokosc && nz >= 0 && nz < dlugosc)
                    {
                        if (target_floor.dungeon[nx][nz] == 1 && !is_inside_any_room(nx, nz))
                        {
                            if (corridor_visited.find({ nx, nz }) == corridor_visited.end())
                            {
                                corridor_visited[{ nx, nz }] = d + 1;
                                q.push({ nx, nz });
                            }
                        }
                    }
                }
            }
        }

        for (const auto& [coord, d] : corridor_visited)
        {
            int cx = coord.first;
            int cz = coord.second;
            if (!target_floor.is_tile_solid(cx, cz) && target_floor.is_tile_free(cx, cz))
            {
                candidates.push_back({ cx, cz, true });
            }
        }
    }

    // Fallback gdy brak pol z marginesem i korytarzy (np. maly pokoj 2x2)
    if (candidates.empty())
    {
        for (int x = rx; x < rx + rw; ++x)
        {
            for (int z = rz; z < rz + rh; ++z)
            {
                if (x >= 0 && x < szerokosc && z >= 0 && z < dlugosc)
                {
                    if (!target_floor.is_tile_solid(x, z) && target_floor.is_tile_free(x, z))
                    {
                        candidates.push_back({ x, z, false });
                    }
                }
            }
        }
    }

    if (candidates.empty())
    {
        return fallback_pos;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    const auto& chosen = candidates[dist(gen)];

    // Dokladny srodek kafelka
    float posX = (chosen.gx + 0.5f) * tile_size;
    float posZ = (chosen.gz + 0.5f) * tile_size;
    return Vector3{ posX, enemy_y, posZ };
}

Vector3 exploration::set_player_pos(int room_id, int floor_id)
{
    Node* room_ptr = this->get_room_by_id(room_id, floor_id);
    if (room_ptr == nullptr)
    {
        return { 0.0f, 1.5f, 0.0f };
    }

    float player_y = 1.5f;
    float centerX = room_ptr->room_x + (room_ptr->room_width / 2.0f);
    float centerZ = room_ptr->room_y + (room_ptr->room_length / 2.0f);

    return Vector3{ centerX * 2.0f, player_y, centerZ * 2.0f };
}

void exploration::spawn_world_item(item* it, Vector3 pos)
{
    if (it == nullptr)
    {
        return;
    }
    const Model* model = nullptr;

    std::unique_ptr<item> item_ptr(it);

    auto new_world_item = std::make_unique<world_item>(
        pos,
        model,
        std::move(item_ptr),
        1.0f,
        0.0f
    );
    floors[current_floor_id].world_objects.push_back(std::move(new_world_item));
}

void exploration::spawn_loot_bag(Vector3 pos, const std::vector<item*>& items)
{
    if (items.empty()) return;

    std::vector<std::unique_ptr<item>> container;
    container.reserve(items.size());

    for (item* it : items)
    {
        if (it != nullptr)
        {
            container.push_back(std::unique_ptr<item>(it));
        }
    }

    if (container.empty()) return;

    int slots = static_cast<int>(container.size());
    auto bag = std::make_unique<p_drop>(pos, nullptr, slots, std::move(container));

    floors[current_floor_id].world_objects.push_back(std::move(bag));
}