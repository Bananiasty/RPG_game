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
    Node new_node;

    new_node.left_id = config.left;
    new_node.right_id = config.right;
    new_node.previous_id = config.previous;

    new_node.room_x = (int)config.dungeon_pos.x;
    new_node.room_y = (int)config.dungeon_pos.y;

    new_node.room_width = (int)config.room_size.x;
    new_node.room_length = (int)config.room_size.y;

    new_node.spawn_chest = config.s_chest;
    new_node.enemy = nullptr;

    world_map[config.id] = new_node;

    if (config.enemy_id != -1)
    {
        enemy* e = get_enemy_by_id(config.enemy_id);
        if (e != nullptr)
        {
            Vector3 pos = set_enemy_pos(config.enemy_id, config.id);

            e->set_position(pos);
            world_map[config.id].enemy = e;
        }
    }
    else
    {
        new_node.enemy = nullptr;
    }

    

}





void exploration::generate_dungeon()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 100);
    

    auto i = world_map.begin();

    while (i != world_map.end())
    {
        Node& this_Node = i->second;

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

        //POKOJE
        for (int current_x = this_Node.room_x; current_x < (this_Node.room_x + this_Node.room_width); current_x++)
        {

            for (int current_y = this_Node.room_y; current_y < (this_Node.room_y + this_Node.room_length); current_y++)
            {
                if (current_x >= 0 && current_x < szerokosc && current_y >= 0 && current_y < dlugosc)
                {
                    dungeon[current_x][current_y] = 1;
                }

            }
        }

        //KORYTARZE
        if (this_Node.left_id != -1)
        {
            Node& Node_B = world_map[this_Node.left_id];
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
                        dungeon[x][start_y] = 1;
                    }

                    if (szerokosc_korytarza == 3)
                    {
                        if (start_y + 1 >= 0 && start_y + 1 < dlugosc) dungeon[x][start_y + 1] = 1;
                        if (start_y - 1 >= 0 && start_y - 1 < dlugosc) dungeon[x][start_y - 1] = 1;
                    }
                    else if (szerokosc_korytarza == 2)
                    {

                        if (start_y + 1 >= 0 && start_y + 1 < dlugosc) dungeon[x][start_y + 1] = 1;
                    }
                }

            }

            int minY = std::min(start_y, end_y);
            int maxY = std::max(start_y, end_y);
            for (int y = minY; y <= maxY; y++)
            {
                if (y >= 0 && y < dlugosc)
                {
                    if (end_x >= 0 && end_x < szerokosc) dungeon[end_x][y] = 1;

                    if (szerokosc_korytarza == 3)
                    {
                        if (end_x + 1 >= 0 && end_x + 1 < szerokosc) dungeon[end_x + 1][y] = 1;
                        if (end_x - 1 >= 0 && end_x - 1 < szerokosc) dungeon[end_x - 1][y] = 1;
                    }
                    else if (szerokosc_korytarza == 2)
                    {
                        if (end_x + 1 >= 0 && end_x + 1 < szerokosc) dungeon[end_x + 1][y] = 1;
                    }
                }
            }
        }

        if (this_Node.right_id != -1)
        {
            Node& Node_B = world_map[this_Node.right_id];
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
                        dungeon[x][start_y] = 1;
                    }

                    if (szerokosc_korytarza == 3)
                    {
                        if (start_y + 1 >= 0 && start_y + 1 < dlugosc) dungeon[x][start_y + 1] = 1;
                        if (start_y - 1 >= 0 && start_y - 1 < dlugosc) dungeon[x][start_y - 1] = 1;
                    }
                    else if (szerokosc_korytarza == 2)
                    {
                        if (start_y + 1 >= 0 && start_y + 1 < dlugosc) dungeon[x][start_y + 1] = 1;
                    }
                }
            }
            int minY = std::min(start_y, end_y);
            int maxY = std::max(start_y, end_y);

            for (int y = minY; y <= maxY; y++)
            {
                if (y >= 0 && y < dlugosc)
                {
                    if (end_x >= 0 && end_x < szerokosc) dungeon[end_x][y] = 1;

                    if (szerokosc_korytarza == 3)
                    {
                        if (end_x + 1 >= 0 && end_x + 1 < szerokosc) dungeon[end_x + 1][y] = 1;
                        if (end_x - 1 >= 0 && end_x - 1 < szerokosc) dungeon[end_x - 1][y] = 1;
                    }
                    else if (szerokosc_korytarza == 2)
                    {
                        if (end_x + 1 >= 0 && end_x + 1 < szerokosc) dungeon[end_x + 1][y] = 1;
                    }
                }
            }
        }

      i++;
    }
    world_chests.clear();
    for (auto& [id, node] : world_map)
    {
        if (node.spawn_chest != nullptr)
        {
            world_chests.push_back(node.spawn_chest);
        }
    }
    
}





void exploration::delete_enemy(enemy* target)
{
    if (target != nullptr)
    {
        std::erase(enemy_pool, target);
        delete target;

        bohater.current_enemy = nullptr;
    }
}

