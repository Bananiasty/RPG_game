#include <random>
#include <vector>
#include <memory>
#include "struct.h"
#include "inventory_class.h"
#include "items.h"
#include "gamestates.h"
#include "character.h"

static int rand_chest_slots()
{
    int max = 5;
    int min = 1;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}

std::unique_ptr<object> exploration::rand_loot(enemy* target_enemy, Vector3 chest_pos)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    if (item_pool.empty()) return nullptr;

    std::uniform_int_distribution<> distr(0, (int)item_pool.size() - 1);

    int slots_number = rand_chest_slots();
    std::vector<std::unique_ptr<item>> drawn_items;

    for (int i = 0; i < slots_number; i++)  
    {
        item* cloned = item_pool[distr(gen)]->clone();
        drawn_items.emplace_back(cloned);
    }

    if (target_enemy != nullptr)
    {
        auto db = std::make_unique<dead_body>(target_enemy, nullptr, slots_number, std::move(drawn_items));
        db->enemy_ptr = target_enemy;
        return db;
    }
    else
    {
        auto c = std::make_unique<chest>(chest_pos, &objects.m_chest, slots_number, std::move(drawn_items));
        return c;
    }
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


