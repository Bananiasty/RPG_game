#include "gamestates.h"
#include "character.h"


enemy& battle::get_enemy()
{
    return e_ref;
}

int battle::get_player_hp()
{
    return p_ref.get_health();
}

int battle::get_enemy_hp()
{
    return e_ref.get_health();
}
int battle::get_enemy_max_hp()
{
    return e_ref.get_max_health();
}

std::string battle::get_enemy_name()
{
    return e_ref.get_name();
}



Node* exploration::get_node(int room_id)
{
    if (floors.empty() || current_floor_id < 0 || current_floor_id >= static_cast<int>(floors.size()))
    {
        return nullptr;
    }

    auto& current_map = floors[current_floor_id].world_map;
    auto it = current_map.find(room_id);
    if (it != current_map.end())
    {
        return &it->second;
    }
    return nullptr;
}

enemy* exploration::get_current_enemy()
{
    if (bohater.current_enemy != nullptr)
    {
        return bohater.current_enemy;
    }

    Node* current_node = get_node(current_node_id);
    if (current_node != nullptr && current_node->enemy_id != -1)
    {
        return get_enemy_by_id(current_node->enemy_id);
    }

    return nullptr;
}

int exploration::get_enemy_dif()
{
    if (bohater.current_enemy != nullptr)
    {
        return bohater.current_enemy->get_dif();
    }

    Node* current_node = get_node(current_node_id);
    if (current_node != nullptr && current_node->enemy_id != -1)
    {
        enemy* e = get_enemy_by_id(current_node->enemy_id);
        if (e != nullptr)
        {
            int dif = e->get_dif();
            delete e;
            return dif;
        }
    }

    return 0;
}

enemy* exploration::get_enemy_by_id(int id)
{
    for (auto* e : enemy_pool)
    {
        if (e != nullptr && e->get_id() == id)
        {
            return e->clone();
        }
    }
    return nullptr;
}

Node* exploration::get_room_by_id(int room_id, int floor_id)
{
    if (floor_id < 0 || floor_id >= static_cast<int>(floors.size()))
    {
        return nullptr;
    }

    auto& floor_map = floors[floor_id].world_map;
    auto it = floor_map.find(room_id);
    if (it != floor_map.end())
    {
        return &(it->second);
    }

    return nullptr;
}