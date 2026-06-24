#include <random>
#include <vector>
#include "struct.h"
#include "inventory_class.h"
#include "items.h"
#include "gamestates.h"
#include "character.h"

int exploration::rand_chest_slots(enemy* e)
{
    int max = e->get_dif();
    int min = max - 1;
    if (max < 1) max = 1;
    if (min < 1) min = 1;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}

chest* exploration::rand_loot(enemy* e)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, (int)item_pool.size() - 1);

    int slots_number = (e != nullptr) ? rand_chest_slots(e) : 1;
    std::vector<item*> drawn_items;

    for (int i = 0; i < slots_number; i++)
    {
        drawn_items.push_back(item_pool[distr(gen)]->clone());
    }

    return new chest(drawn_items, slots_number);
}

chest::~chest()
{
    for (item* i : chest_loot)
    {
        delete i;
    }
    chest_loot.clear();
}

void chest_drop::collect_loot()
{
    if (chest_ptr == nullptr) return;
    for (item* it : chest_ptr->chest_loot)
    {
        if (it != nullptr)
        {
            exp->bohater.bag->add_item(it);
        }
    }

    chest_ptr->chest_loot.clear();
}

void chest_drop::discard_chest()
{
    chest* current_chest = exp->world_map[exp->current_node_id].spawn_chest;
    if (current_chest != nullptr)
    {
        delete current_chest;
        exp->world_map[exp->current_node_id].spawn_chest = nullptr;
    }
    exp->world_map[exp->current_node_id].current_event = nullptr;
    delete this;
}
	