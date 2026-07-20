#include <random>
#include <vector>
#include "struct.h"
#include "inventory_class.h"
#include "items.h"
#include "gamestates.h"
#include "character.h"

int exploration::rand_chest_slots()
{
    int max = 5;
    int min = 1;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}

chest* exploration::rand_loot(enemy* target_enemy, Vector3 chest_pos)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, (int)item_pool.size() - 1);

    int slots_number = rand_chest_slots();
    std::vector<item*> drawn_items;

    for (int i = 0; i < slots_number; i++)
    {
        drawn_items.push_back(item_pool[distr(gen)]->clone());
    }

    chest* new_drop = nullptr;

    if (target_enemy != nullptr)
    {
        new_drop = new chest(drawn_items, slots_number, target_enemy->get_position());

        new_drop->enemy_ptr = target_enemy;
        new_drop->enemy_loot = drawn_items;
    }
    else
    {
        new_drop = new chest(drawn_items, slots_number, chest_pos);
        new_drop->enemy_ptr = nullptr;
    }

    return new_drop;
}


chest::~chest()
{
    for (item* i : chest_loot)
    {
        if (i != nullptr)
        {
            delete i;
        }
        
    }
    chest_loot.clear();
    for (item* i : enemy_loot)
    {
        if (i != nullptr)
        {
            delete i;
        }
    }
    enemy_loot.clear();
}

void chest_drop::collect_chest_loot()
{
    chest* real_chest = exp->world_map[exp->current_node_id].spawn_chest;
    
    if (real_chest == nullptr) return;
    for (item* it : real_chest->chest_loot)
    {
        exp->bohater.bag->add_item(it);
    }

    real_chest->chest_loot.clear();
}

void chest_drop::discard_chest()
{
    is_chest_open = false;
}

void enemy_loot::collect_enemy_loot()
{
    if (this->chest_ptr == nullptr)
    {
        return;
    }
    for (size_t i = 0; i < this->chest_ptr->enemy_loot.size(); i++)
    {
        item* current_item = this->chest_ptr->enemy_loot[i];

        if (current_item != nullptr)
        {
            p_ref.bag->add_item(current_item);

            this->chest_ptr->enemy_loot[i] = nullptr;
        }
    }
    this->chest_ptr->enemy_loot.clear();

    is_loot_open = false;
}

void enemy_loot::discard_enemy_items()
{
    is_loot_open = false;

}
	