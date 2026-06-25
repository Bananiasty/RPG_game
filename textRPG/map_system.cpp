#include <iostream>
#include <map>
#include <string>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"

void exploration::add_Node(int id, std::string desc, int left, int right, int previous, bool disc, int posX, int posY, enemy* e, chest* s_chest)
{
    Node temp_node;
    temp_node.description = desc;
    temp_node.left_id = left;
    temp_node.right_id = right;
    temp_node.enemy = e;
    temp_node.previous_id = previous;
    temp_node.spawn_chest = s_chest;
    temp_node.discovered = disc;
    temp_node.positionX = posX;
    temp_node.positionY = posY;

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
void exploration::delete_dead_enemies()
{
    if (world_map[current_node_id].enemy != nullptr)
    {
        delete world_map[current_node_id].enemy;
        world_map[current_node_id].enemy = nullptr;

        bohater.current_enemy = nullptr;
    }
}

