#include <random>
#include <vector>
#include <memory>
#include "struct.h"
#include "inventory_class.h"
#include "items.h"
#include "gamestates.h"
#include "character.h"

int drop_object::rand_drop_slots()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 4);
    return dist(gen);
}

std::vector<std::unique_ptr<item>> exploration::rand_loot(enemy* target_enemy, int slots_number)
{
    if (item_pool.empty()) return {};

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, (int)item_pool.size() - 1);

    std::vector<std::unique_ptr<item>> drawn_items;
    drawn_items.reserve(slots_number);

    for (int i = 0; i < slots_number; i++)
    {
        item* cloned = item_pool[distr(gen)]->clone();
        drawn_items.emplace_back(cloned);
    }

    return drawn_items;
}


void loot_event::collect_item(size_t index)
{       
    if (target_container == nullptr) return;
    drop_object* drop = dynamic_cast<drop_object*>(target_container);
    if (drop == nullptr) return;
    if (index >= drop->drop_loot.size()) return;

    auto& up = drop->drop_loot[index];
    if (up)
    {
        p_ref.bag->add_item(up.release());
        drop->drop_loot.erase(drop->drop_loot.begin() + index);
        p_ref.sort_bag();
    }
}

void loot_event::collect_all()
{
    if (target_container == nullptr) return;
    drop_object* drop = dynamic_cast<drop_object*>(target_container);
    if (drop == nullptr) return;

    for (auto& up : drop->drop_loot)
    {
        if (up)
        {
            p_ref.bag->add_item(up.release());
        }
    }
    drop->drop_loot.clear();
    p_ref.sort_bag();
    is_active = false;
}

void loot_event::close_event()
{
    is_active = false;
}

void loot_event::update()
{
}


