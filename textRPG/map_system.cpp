#include <iostream>
#include <map>
#include <string>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "graphics.h"
#include <random>



void exploration::add_Node(const NodeConfig& config)
{
    if (config.floor_id < 0 || config.floor_id >= static_cast<int>(floors.size()))
    {
        return;
    }

    auto& current_map = floors[config.floor_id].world_map;

    Node new_node;
    new_node.left_id = config.left;
    new_node.right_id = config.right;
    new_node.previous_id = config.previous;
    new_node.discovered = config.discovered;

    new_node.room_x = static_cast<int>(config.dungeon_pos.x);
    new_node.room_y = static_cast<int>(config.dungeon_pos.y);

    new_node.room_width = static_cast<int>(config.room_size.x);
    new_node.room_length = static_cast<int>(config.room_size.y);

    new_node.spawn_chest = config.s_chest;
    new_node.enemy = nullptr;

    // 1. Zapis węzła do mapy piętra
    current_map[config.room_id] = new_node;

    // 2. Pozycjonowanie wroga bezpośrednio z parametrów config
    if (config.enemy_id != -1)
    {
        enemy* e = get_enemy_by_id(config.enemy_id);
        if (e != nullptr)
        {
            float centerX = config.dungeon_pos.x + (config.room_size.x / 2.0f);
            float centerZ = config.dungeon_pos.y + (config.room_size.y / 2.0f);
            float enemy_y = e->get_position().y;

            Vector3 pos = { centerX * 2.0f, enemy_y, centerZ * 2.0f };
            e->set_position(pos);

            current_map[config.room_id].enemy = e;
        }
    }
}





void exploration::generate_floor(int floor_id)
{
    /*if (floor_id < 0 || floor_id >= static_cast<int>(floors.size()))
    {
        return;
    }*/

    dungeon_floor& target_floor = floors[floor_id];

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 100);

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
        int start_y = this_Node.room_y + (int)std::floor(this_Node.room_length / 2.0f);

        // POKOJE
        for (int current_x = this_Node.room_x; current_x < (this_Node.room_x + this_Node.room_width); current_x++)
        {
            for (int current_y = this_Node.room_y; current_y < (this_Node.room_y + this_Node.room_length); current_y++)
            {
                if (current_x >= 0 && current_x < szerokosc && current_y >= 0 && current_y < dlugosc)
                {
                    target_floor.dungeon[current_x][current_y] = 1;
                }
            }
        }

        // KORYTARZ LEWY
        if (this_Node.left_id != -1 && target_floor.world_map.find(this_Node.left_id) != target_floor.world_map.end())
        {
            Node& Node_B = target_floor.world_map[this_Node.left_id];
            int end_x = Node_B.room_x + (Node_B.room_width / 2);
            int end_y = Node_B.room_y + (Node_B.room_length / 2);

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
            int end_y = Node_B.room_y + (Node_B.room_length / 2);

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

    target_floor.world_loot.clear();
    for (auto& [id, node] : target_floor.world_map)
    {
        if (node.spawn_chest != nullptr)
        {
            target_floor.world_loot.push_back(node.spawn_chest);
        }
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
        for (auto& [id, node] : floor.world_map)
        {
            if (node.enemy == target)
            {
                node.enemy = nullptr;
            }
        }
    }

    std::erase(enemy_pool, target);

    delete target;

    bohater.current_enemy = nullptr;
}

