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
    auto it = world_map.find(room_id);
    if (it != world_map.end())
    {
        return &it->second;
    }
    return nullptr;
}

enemy* exploration::get_current_enemy()
{
    return world_map[current_node_id].enemy;
}

int exploration::get_enemy_dif()
{
    if (world_map[current_node_id].enemy != nullptr)
    {
        return world_map[current_node_id].enemy->get_dif();
    }
    return 0;
}

enemy* exploration::get_enemy_by_id(int id)
{
    for (auto* e : enemy_pool)
    {
        if (e != nullptr && e->get_id() == id)
            return e->clone();
    }
    return nullptr;
}

Node* exploration::get_room_by_id(int id)
{
    auto it = world_map.find(id);
    if (it != world_map.end())
    {
        return &it->second;
    }
    return nullptr;
}
