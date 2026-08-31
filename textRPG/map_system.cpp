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
    Node node;
    node.left_id = config.left;
    node.right_id = config.right;
    node.previous_id = config.previous;
    node.discovered = config.discovered;

    node.room_x = (int)config.dungeon_pos.x;
    node.room_y = (int)config.dungeon_pos.y;
    node.room_width = (int)config.room_size.x;
    node.room_length = (int)config.room_size.y;
    node.enemy_id = config.enemy_id;

    floors[config.floor_id].world_map[config.room_id] = node;

    for (const auto& prop_info : config.props)
    {
        auto obj = create_world_object(prop_info);
        if (obj != nullptr)
        {
            floors[config.floor_id].world_objects.push_back(std::move(obj));
        }
    }
    if (config.enemy_id != -1)
    {
        enemy* spawned_enemy = get_enemy_by_id(config.enemy_id);
        if (spawned_enemy != nullptr)
        {
            float currentY = spawned_enemy->get_position().y;
            float posX = (config.dungeon_pos.x + config.room_size.x / 2.0f) * 2.0f;
            float posZ = (config.dungeon_pos.y + config.room_size.y / 2.0f) * 2.0f;
            spawned_enemy->set_position({ posX, currentY, posZ });

            floors[config.floor_id].active_enemies.push_back(spawned_enemy);
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

