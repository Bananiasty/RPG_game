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



Node* exploration::get_current_node()
{
    return &world_map[current_node_id];
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
