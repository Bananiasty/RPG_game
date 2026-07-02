#include <iostream>
#include <map>
#include <string>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include <random>



void exploration::add_Node(int id, int left, int right, int previous, bool disc, int posX, int posY, enemy* e, chest* s_chest, int rx, int rh, int r_width, int r_length)
{
    Node temp_node;
    temp_node.left_id = left;
    temp_node.right_id = right;
    temp_node.enemy = e;
    temp_node.previous_id = previous;
    temp_node.spawn_chest = s_chest;
    temp_node.discovered = disc;
    temp_node.positionX = posX;
    temp_node.positionY = posY;
    temp_node.room_x = rx;
    temp_node.room_y = rh;
    temp_node.room_width = r_width;
    temp_node.room_length = r_length;

    world_map[id] = temp_node;
}
void exploration::map_graph()
{
    auto i = world_map.begin();
    i++;
    while (i != world_map.end())
    {
        Node& this_Node = i->second;
        if (this_Node.previous_id != -1)
        {
            Node& parent = world_map[this_Node.previous_id];

            if (i->first == parent.left_id && parent.left_id != -1)
            {
                this_Node.positionX = parent.positionX - 80;
                this_Node.positionY = parent.positionY - 80;
            }
            else if (i->first == parent.right_id && parent.right_id != -1)
            {
                this_Node.positionX = parent.positionX + 80;
                this_Node.positionY = parent.positionY - 80;
            }
        }
           
        i++;
    }

}



void exploration::generate_map_from_graph()
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

        int start_x = this_Node.room_x + (this_Node.room_width / 2);
        int start_y = this_Node.room_y + (this_Node.room_length / 2);

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
    
}





void exploration::delete_dead_enemies()
{
    if (world_map[current_node_id].enemy != nullptr)
    {
        delete world_map[current_node_id].enemy;
        world_map[current_node_id].enemy = nullptr;

        bohater.current_enemy = nullptr;
    }
}

