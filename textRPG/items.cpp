#include <iostream>
#include <vector>
#include <string>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"

item_type armor::get_type()
{
    return item_type::ARMOR;
}

item_type usable::get_type()
{
    return item_type::USABLE;
}

item_type scroll::get_type()
{
    return item_type::SCROLL;
}

bool armor::is_equipped()
{
    return this->item_equipped;
}

bool helm::is_equipped() { return armor::is_equipped(); }
bool vest::is_equipped() { return armor::is_equipped(); }
bool gauntlets::is_equipped() { return armor::is_equipped(); }
bool boots::is_equipped() { return armor::is_equipped(); }
bool weapon::is_equipped() { return armor::is_equipped(); }

void armor::use(player* c, int index)
{
    if (c != nullptr)
    {
        if (this->item_equipped == false)
        {
            this->item_equipped = true;
            add_player_defense(c, this->defense_stat);
        }
        else
        {
            this->item_equipped = false;
            this->reduce_player_defense(c, this->defense_stat);
        }
    }
}

void helm::use(player* c, int index)
{
    if (this->item_equipped == false && c->helm_slot == true) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_block_chance(c, this->block_chance);
        c->helm_slot = true;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_block_chance(c, this->block_chance);
        c->helm_slot = false;
    }
}

void vest::use(player* c, int index)
{
    if (this->item_equipped == false && c->vest_slot == true) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_health(c, this->add_health);
        c->vest_slot = true;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_health(c, this->add_health);
        c->vest_slot = false;
    }
}

void gauntlets::use(player* c, int index)
{
    if (this->item_equipped == false && c->gauntlets_slot == true) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_crit_chance(c, this->crit_chance_mod);
        c->gauntlets_slot = true;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_crit_chance(c, this->crit_chance_mod);
        c->gauntlets_slot = false;
    }
}

void boots::use(player* c, int index)
{
    if (this->item_equipped == false && c->boots_slot == true) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_dodge_chance(c, this->dodge_chance_mod);
        c->boots_slot = true;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_dodge_chance(c, this->dodge_chance_mod);
        c->boots_slot = false;
    }
}

void weapon::use(player* c, int index)
{
    if (this->item_equipped == false && c->weapon_slot == true) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_damage(c, this->damage_stat);
        c->weapon_slot = true;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_damage(c, this->damage_stat);
        c->weapon_slot = false;
    }
}

//void inventory::del_item(int index)
//{
//    if (index >= 0 && index < items.size())
//    {
//        item* to_remove = items[index];
//        delete to_remove;
//        items.erase(items.begin() + index);
//    }
//}

void usable::use(player* c, int index)
{
    auto& items = c->bag->items;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if (*it == this) 
        {
            delete* it;       
            items.erase(it);  
            break;
        }
    }
}
   


void health_potion::use(player* c, int index)
{
    restore_player_health(c, this->restore_health);
    usable::use(c, index);
}

void mana_potion::use(player* c, int index)
{
    restore_player_mana(c, this->restore_mana);
    usable::use(c, index);
}

void combat_scroll::use(player* c, int index)
{
    if (c->current_enemy != nullptr && c->spell_queued==false)
    {
        c->spell_queued = true;
        c->queued_damage = this->damage;

        c->queued_animation_texture = this->animation_texture;
        c->queued_frame_count = this->frame_count;
        c->queued_frame_time = this->frame_time;
        
        usable::use(c, index);       
    }
}

void inventory::add_item(item* new_item)
{
    if (items.size() < max_slots)
    {
        items.push_back(new_item);
    }
}

std::string item::get_name()
{
    return name;
}

item* helm::clone() const { return new helm(*this); }
item* vest::clone() const { return new vest(*this); }
item* gauntlets::clone() const { return new gauntlets(*this); }
item* weapon::clone() const { return new weapon(*this); }
item* boots::clone() const { return new boots(*this); }
item* health_potion::clone() const { return new health_potion(*this); }
item* mana_potion::clone() const { return new mana_potion(*this); }
item* combat_scroll::clone() const { return new combat_scroll(*this); }