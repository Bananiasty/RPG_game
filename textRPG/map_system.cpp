#include <iostream>
#include <map>
#include <string>
#include <queue>
#include <random>
#include <utility>
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "gamestates.h"
#include "struct.h"
#include "graphics.h"

static void carve_room(dungeon_floor& target_floor, const Node& node, int szerokosc, int dlugosc)
{
    int room_x = node.room_x;
    int room_y = node.room_y;
    int room_width = node.room_width;
    int room_height = node.room_height;

    switch (node.room_shape)
    {
    case Room_type::Rectangle:
    {
        for (int cx = room_x; cx < room_x + room_width; cx++)
        {
            for (int cy = room_y; cy < room_y + room_height; cy++)
            {
                if (cx >= 0 && cx < szerokosc && cy >= 0 && cy < dlugosc)
                {
                    target_floor.dungeon[cx][cy] = 1;
                }
            }
        }
        break;
    }

    case Room_type::Center_pillar:
    {
        // 1. Wytnij cala podloge pokoju
        for (int cx = room_x; cx < room_x + room_width; cx++)
        {
            for (int cy = room_y; cy < room_y + room_height; cy++)
            {
                if (cx >= 0 && cx < szerokosc && cy >= 0 && cy < dlugosc)
                {
                    target_floor.dungeon[cx][cy] = 1;
                }
            }
        }

        // 2. Filar centralny: 1x1 jesli nieparzysty, 2x2 jesli parzysty
        int min_px = (room_width % 2 == 0) ? (room_x + room_width / 2 - 1) : (room_x + room_width / 2);
        int max_px = room_x + room_width / 2;
        int min_py = (room_height % 2 == 0) ? (room_y + room_height / 2 - 1) : (room_y + room_height / 2);
        int max_py = room_y + room_height / 2;

        for (int px = min_px; px <= max_px; px++)
        {
            for (int py = min_py; py <= max_py; py++)
            {
                if (px >= 0 && px < szerokosc && py >= 0 && py < dlugosc)
                {
                    target_floor.dungeon[px][py] = 2;
                    target_floor.occupy_tile(px, py, true);
                }
            }
        }
        break;
    }

    case Room_type::FourPillars:
    {
        // 1. Wytnij cala podloge pokoju
        for (int cx = room_x; cx < room_x + room_width; cx++)
        {
            for (int cy = room_y; cy < room_y + room_height; cy++)
            {
                if (cx >= 0 && cx < szerokosc && cy >= 0 && cy < dlugosc)
                {
                    target_floor.dungeon[cx][cy] = 1;
                }
            }
        }

        // 2. Cztery symetryczne filary rozmieszczone rownomiernie od scian
        int kx = (room_width >= 8) ? 2 : 1;
        int ky = (room_height >= 8) ? 2 : 1;

        int px1 = room_x + kx;
        int px2 = room_x + room_width - 1 - kx;
        int py1 = room_y + ky;
        int py2 = room_y + room_height - 1 - ky;

        const std::pair<int, int> pillars[4] = {
            { px1, py1 },
            { px2, py1 },
            { px1, py2 },
            { px2, py2 }
        };

        for (const auto& [px, py] : pillars)
        {
            if (px >= 0 && px < szerokosc && py >= 0 && py < dlugosc)
            {
                target_floor.dungeon[px][py] = 2;
                target_floor.occupy_tile(px, py, true);
            }
        }
        break;
    }

    case Room_type::Cross:
    {
        int cx = (room_width >= 7) ? (room_width - 3) / 2 : 1;
        int cy = (room_height >= 7) ? (room_height - 3) / 2 : 1;

        for (int dx = 0; dx < room_width; dx++)
        {
            for (int dy = 0; dy < room_height; dy++)
            {
                bool is_top_left = (dx < cx && dy < cy);
                bool is_top_right = (dx >= room_width - cx && dy < cy);
                bool is_bottom_left = (dx < cx && dy >= room_height - cy);
                bool is_bottom_right = (dx >= room_width - cx && dy >= room_height - cy);

                int gx = room_x + dx;
                int gy = room_y + dy;

                if (gx >= 0 && gx < szerokosc && gy >= 0 && gy < dlugosc)
                {
                    if (!(is_top_left || is_top_right || is_bottom_left || is_bottom_right))
                    {
                        target_floor.dungeon[gx][gy] = 1;
                    }
                }
            }
        }
        break;
    }

    case Room_type::LShape:
    {
        int cut_w = room_width / 2;
        int cut_h = room_height / 2;

        for (int dx = 0; dx < room_width; dx++)
        {
            for (int dy = 0; dy < room_height; dy++)
            {
                // Wycinamy prawy gorny rog
                bool in_cut = (dx >= room_width - cut_w && dy < cut_h);

                int gx = room_x + dx;
                int gy = room_y + dy;

                if (gx >= 0 && gx < szerokosc && gy >= 0 && gy < dlugosc)
                {
                    if (!in_cut)
                    {
                        target_floor.dungeon[gx][gy] = 1;
                    }
                }
            }
        }
        break;
    }
    }
}

void exploration::add_Node(const NodeConfig& config)
{
    Node node;
    node.left_id = config.left;
    node.right_id = config.right;
    node.previous_id = config.previous;
    node.discovered = config.discovered;

    node.room_x = (int)config.dungeon_pos.x;
    node.room_y = (int)config.dungeon_pos.y;
    node.room_width = (int)config.room_width;
    node.room_height = (int)config.room_height;
    node.room_shape = config.room_shape;
    determine_room_size(node.room_shape, node.room_width, node.room_height);

    node.enemy_ids = config.enemy_ids;
    node.pending_props = config.props;

    floors[config.floor_id].world_map[config.room_id] = node;

    if (floors[config.floor_id].is_generated)
    {
        carve_room(floors[config.floor_id], node, szerokosc, dlugosc);

        for (const auto& prop_info : node.pending_props)
        {
            spawn_object(prop_info);
        }
        node.pending_props.clear();

        for (int enemy_id : config.enemy_ids)
        {
            if (enemy_id != -1)
            {
                enemy* spawned_enemy = get_enemy_by_id(enemy_id);
                if (spawned_enemy != nullptr)
                {
                    Vector3 pos = set_enemy_pos(enemy_id, config.room_id, config.floor_id);
                    spawned_enemy->set_position(pos);

                    floors[config.floor_id].active_enemies.push_back(spawned_enemy);
                }
            }
        }
    }
}

int exploration::find_deepest_room(int floor_id, int start_room_id)
{
    if (floor_id < 0 || floor_id >= static_cast<int>(floors.size()))
    {
        return start_room_id;
    }

    const auto& world_map = floors[floor_id].world_map;
    if (world_map.empty())
    {
        return start_room_id;
    }

    std::map<int, std::vector<int>> adj;
    for (const auto& [id, node] : world_map)
    {
        if (node.left_id != -1 && world_map.find(node.left_id) != world_map.end())
        {
            adj[id].push_back(node.left_id);
            adj[node.left_id].push_back(id);
        }
        if (node.right_id != -1 && world_map.find(node.right_id) != world_map.end())
        {
            adj[id].push_back(node.right_id);
            adj[node.right_id].push_back(id);
        }
        if (node.previous_id != -1 && world_map.find(node.previous_id) != world_map.end())
        {
            adj[id].push_back(node.previous_id);
            adj[node.previous_id].push_back(id);
        }
    }

    std::map<int, int> distance;
    std::queue<int> q;

    distance[start_room_id] = 0;
    q.push(start_room_id);

    int deepest_room = start_room_id;
    int max_dist = 0;

    while (!q.empty())
    {
        int curr = q.front();
        q.pop();

        int curr_d = distance[curr];
        if (curr_d > max_dist)
        {
            max_dist = curr_d;
            deepest_room = curr;
        }

        for (int neighbor : adj[curr])
        {
            if (distance.find(neighbor) == distance.end())
            {
                distance[neighbor] = curr_d + 1;
                q.push(neighbor);
            }
        }
    }

    // Fallback: jesli graf byl niespojny lub dystans 0, znajdz najdalszy po wspolrzednych
    if (deepest_room == start_room_id && world_map.size() > 1)
    {
        float max_coord_dist = -1.0f;
        auto start_it = world_map.find(start_room_id);
        Vector2 start_pos = (start_it != world_map.end()) ? Vector2{ (float)start_it->second.room_x, (float)start_it->second.room_y } : Vector2{ 0, 0 };

        for (const auto& [id, node] : world_map)
        {
            if (id == start_room_id) continue;
            float dx = (float)node.room_x - start_pos.x;
            float dy = (float)node.room_y - start_pos.y;
            float dist = dx * dx + dy * dy;
            if (dist > max_coord_dist)
            {
                max_coord_dist = dist;
                deepest_room = id;
            }
        }
    }

    return deepest_room;
}

void exploration::ensure_trapdoor_in_deepest_room(int floor_id)
{
    if (floor_id < 0 || floor_id >= static_cast<int>(floors.size()))
    {
        return;
    }

    auto& target_floor = floors[floor_id];
    if (target_floor.world_map.empty())
    {
        return;
    }

    int deepest_id = find_deepest_room(floor_id, 1);
    auto deepest_it = target_floor.world_map.find(deepest_id);
    if (deepest_it == target_floor.world_map.end())
    {
        return;
    }

    Node& deepest_node = deepest_it->second;

    ObjectSpawnInfo found_trapdoor;
    bool has_trapdoor = false;

    for (auto& [id, node] : target_floor.world_map)
    {
        for (auto it = node.pending_props.begin(); it != node.pending_props.end(); )
        {
            if (it->type == ObjectType::Trapdoor)
            {
                found_trapdoor = *it;
                has_trapdoor = true;
                it = node.pending_props.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    if (has_trapdoor)
    {
        found_trapdoor.room_pos = Vector2{ (float)deepest_node.room_x, (float)deepest_node.room_y };
        found_trapdoor.room_size = Vector2{ (float)deepest_node.room_width, (float)deepest_node.room_height };
        deepest_node.pending_props.push_back(found_trapdoor);
    }
    else if (floor_id + 1 < static_cast<int>(floors.size()))
    {
        ObjectSpawnInfo auto_trapdoor = ObjectSpawnInfo::create_trapdoor(
            Vector2{ (float)deepest_node.room_x, (float)deepest_node.room_y },
            Vector2{ (float)deepest_node.room_width, (float)deepest_node.room_height },
            floor_id + 1
        );
        deepest_node.pending_props.push_back(auto_trapdoor);
    }
}

void exploration::generate_floor(int floor_id)
{
    if (floor_id < 0 || floor_id >= static_cast<int>(floors.size()))
    {
        return;
    }

    dungeon_floor& target_floor = floors[floor_id];
    if (target_floor.is_generated)
    {
        return;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 100);

    // 1. KORYTARZE
    for (auto& [id, this_Node] : target_floor.world_map)
    {
        int rand_number = distr(gen);

        int szerokosc_korytarza = 1;
        if (rand_number <= 25) {
            szerokosc_korytarza = 3;
        }
        else if (rand_number >= 95) {
            szerokosc_korytarza = 2;
        }

        int start_x = this_Node.room_x + (int)std::floor(this_Node.room_width / 2.0f);
        int start_y = this_Node.room_y + (int)std::floor(this_Node.room_height / 2.0f);

        // KORYTARZ LEWY
        if (this_Node.left_id != -1 && target_floor.world_map.find(this_Node.left_id) != target_floor.world_map.end())
        {
            Node& Node_B = target_floor.world_map[this_Node.left_id];
            int end_x = Node_B.room_x + (Node_B.room_width / 2);
            int end_y = Node_B.room_y + (Node_B.room_height / 2);

            int minX = std::min(start_x, end_x);
            int maxX = std::max(start_x, end_x);

            for (int x = minX; x <= maxX; x++)
            {
                if (x >= 0 && x < szerokosc)
                {
                    if (start_y >= 0 && start_y < dlugosc)
                    {
                        target_floor.dungeon[x][start_y] = 1;
                    }

                    if (szerokosc_korytarza == 3)
                    {
                        if (start_y + 1 >= 0 && start_y + 1 < dlugosc) target_floor.dungeon[x][start_y + 1] = 1;
                        if (start_y - 1 >= 0 && start_y - 1 < dlugosc) target_floor.dungeon[x][start_y - 1] = 1;
                    }
                    else if (szerokosc_korytarza == 2)
                    {
                        if (start_y + 1 >= 0 && start_y + 1 < dlugosc) target_floor.dungeon[x][start_y + 1] = 1;
                    }
                }
            }

            int minY = std::min(start_y, end_y);
            int maxY = std::max(start_y, end_y);
            for (int y = minY; y <= maxY; y++)
            {
                if (y >= 0 && y < dlugosc)
                {
                    if (end_x >= 0 && end_x < szerokosc) target_floor.dungeon[end_x][y] = 1;

                    if (szerokosc_korytarza == 3)
                    {
                        if (end_x + 1 >= 0 && end_x + 1 < szerokosc) target_floor.dungeon[end_x + 1][y] = 1;
                        if (end_x - 1 >= 0 && end_x - 1 < szerokosc) target_floor.dungeon[end_x - 1][y] = 1;
                    }
                    else if (szerokosc_korytarza == 2)
                    {
                        if (end_x + 1 >= 0 && end_x + 1 < szerokosc) target_floor.dungeon[end_x + 1][y] = 1;
                    }
                }
            }
        }

        // KORYTARZ PRAWY
        if (this_Node.right_id != -1 && target_floor.world_map.find(this_Node.right_id) != target_floor.world_map.end())
        {
            Node& Node_B = target_floor.world_map[this_Node.right_id];
            int end_x = Node_B.room_x + (Node_B.room_width / 2);
            int end_y = Node_B.room_y + (Node_B.room_height / 2);

            int minX = std::min(start_x, end_x);
            int maxX = std::max(start_x, end_x);

            for (int x = minX; x <= maxX; x++)
            {
                if (x >= 0 && x < szerokosc)
                {
                    if (start_y >= 0 && start_y < dlugosc)
                    {
                        target_floor.dungeon[x][start_y] = 1;
                    }

                    if (szerokosc_korytarza == 3)
                    {
                        if (start_y + 1 >= 0 && start_y + 1 < dlugosc) target_floor.dungeon[x][start_y + 1] = 1;
                        if (start_y - 1 >= 0 && start_y - 1 < dlugosc) target_floor.dungeon[x][start_y - 1] = 1;
                    }
                    else if (szerokosc_korytarza == 2)
                    {
                        if (start_y + 1 >= 0 && start_y + 1 < dlugosc) target_floor.dungeon[x][start_y + 1] = 1;
                    }
                }
            }

            int minY = std::min(start_y, end_y);
            int maxY = std::max(start_y, end_y);

            for (int y = minY; y <= maxY; y++)
            {
                if (y >= 0 && y < dlugosc)
                {
                    if (end_x >= 0 && end_x < szerokosc) target_floor.dungeon[end_x][y] = 1;

                    if (szerokosc_korytarza == 3)
                    {
                        if (end_x + 1 >= 0 && end_x + 1 < szerokosc) target_floor.dungeon[end_x + 1][y] = 1;
                        if (end_x - 1 >= 0 && end_x - 1 < szerokosc) target_floor.dungeon[end_x - 1][y] = 1;
                    }
                    else if (szerokosc_korytarza == 2)
                    {
                        if (end_x + 1 >= 0 && end_x + 1 < szerokosc) target_floor.dungeon[end_x + 1][y] = 1;
                    }
                }
            }
        }
    }

    //POKOJE (Wycinamy pokoje zgodnie z ich room_shape po korytarzach, aby zachowac filary)
    for (auto& [id, this_Node] : target_floor.world_map)
    {
        carve_room(target_floor, this_Node, szerokosc, dlugosc);
    }

    //WEJSCIA (Rezerwujemy kafelki wejsc do pokojow)
    for (const auto& [id, node] : target_floor.world_map)
    {
        int rx = node.room_x;
        int rz = node.room_y;
        int rw = node.room_width;
        int rh = node.room_height;

        for (int x = rx; x < rx + rw; x++)
        {
            if (rz - 1 >= 0 && target_floor.dungeon[x][rz - 1] == 1 && target_floor.dungeon[x][rz] == 1)
            {
                target_floor.occupy_tile(x, rz, false);
            }
            if (rz + rh < dlugosc && target_floor.dungeon[x][rz + rh] == 1 && target_floor.dungeon[x][rz + rh - 1] == 1)
            {
                target_floor.occupy_tile(x, rz + rh - 1, false);
            }
        }

        for (int z = rz; z < rz + rh; z++)
        {
            if (rx - 1 >= 0 && target_floor.dungeon[rx - 1][z] == 1 && target_floor.dungeon[rx][z] == 1)
            {
                target_floor.occupy_tile(rx, z, false);
            }
            if (rx + rw < szerokosc && target_floor.dungeon[rx + rw][z] == 1 && target_floor.dungeon[rx + rw - 1][z] == 1)
            {
                target_floor.occupy_tile(rx + rw - 1, z, false);
            }
        }
    }
    ensure_trapdoor_in_deepest_room(floor_id);

    // 5. Spawnowanie obiektow / rekwizytow
    int prev_floor = current_floor_id;
    current_floor_id = floor_id;

    for (auto& [id, node] : target_floor.world_map)
    {
        for (const auto& prop_info : node.pending_props)
        {
            spawn_object(prop_info);
        }
        node.pending_props.clear();
    }

    current_floor_id = prev_floor;

    //Spawnowanie przeciwnikow
    for (auto& [id, node] : target_floor.world_map)
    {
        for (int enemy_id : node.enemy_ids)
        {
            if (enemy_id != -1)
            {
                enemy* spawned_enemy = get_enemy_by_id(enemy_id);
                if (spawned_enemy != nullptr)
                {
                    Vector3 pos = set_enemy_pos(enemy_id, id, floor_id);
                    spawned_enemy->set_position(pos);
                    target_floor.active_enemies.push_back(spawned_enemy);
                }
            }
        }
    }

    target_floor.is_generated = true;
}

void exploration::change_floor(int new_floor_id)
{
    current_floor_id = new_floor_id;
    generate_floor(current_floor_id);
    Vector3 new_player_pos = set_player_pos(1, current_floor_id);

    camera.position = new_player_pos;
    camera.target = { new_player_pos.x + 1.0f, new_player_pos.y, new_player_pos.z };
    bohater.position = new_player_pos;

    if (active_ui_event != nullptr)
    {
        delete active_ui_event;
        active_ui_event = nullptr;
    }
}

void exploration::delete_enemy(enemy* target)
{
    if (target == nullptr)
    {
        return;
    }

    for (auto& floor : floors)
    {
        std::erase(floor.active_enemies, target);
    }

    if (bohater.current_enemy == target)
    {
        bohater.current_enemy = nullptr;
    }

    delete target;
}
