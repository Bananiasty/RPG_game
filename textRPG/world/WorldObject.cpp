#include "world/WorldObject.h"
#include "gamestates.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include <random>

int loot_object::rand_drop_slots()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 4);
    return dist(gen);
}

void loot_object::interact(exploration& game, player& player_character)
{
    if (!drop_loot.empty())
    {
        game.active_ui_event = new loot_event(&game, player_character, this);
    }
}

void trapdoor::interact(exploration& game, player& player_character)
{
    if (!is_open)
    {
        is_open = true;
    }
    else if (open_angle >= 75.0f && target_floor_id != -1)
    {
        game.change_floor(target_floor_id);
    }
}

void world_item::interact(exploration& game, player& player_character)
{
    item* raw_item = this->stored_item.release();

    if (raw_item != nullptr)
    {
        player_character.bag->add_item(raw_item);
    }

    this->is_destroyed = true;
}
