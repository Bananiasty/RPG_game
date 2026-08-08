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

item_type other_item::get_type()
{
    return item_type::ITEM;
}

item_type food::get_type()
{
    return item_type::FOOD;
}

item_type scroll::get_type()
{
    return item_type::BOOK;
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
    if (this->item_equipped == false && c->equipped_helm != nullptr) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_block_chance(c, this->block_chance);
        c->equipped_helm = this;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_block_chance(c, this->block_chance);
        c->equipped_helm = nullptr;
    }
}

void vest::use(player* c, int index)
{
    if (this->item_equipped == false && c->equipped_vest != nullptr) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_health(c, this->add_health);
        c->equipped_vest = this;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_health(c, this->add_health);
        c->equipped_vest = nullptr;
    }
}

void gauntlets::use(player* c, int index)
{
    if (this->item_equipped == false && c->equipped_gauntlets != nullptr) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_crit_chance(c, this->crit_chance_mod);
        c->equipped_gauntlets = this;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_crit_chance(c, this->crit_chance_mod);
        c->equipped_gauntlets = nullptr;
    }
}

void boots::use(player* c, int index)
{
    if (this->item_equipped == false && c->equipped_boots != nullptr) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {
        add_player_dodge_chance(c, this->dodge_chance_mod);
        c->equipped_boots = this;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_dodge_chance(c, this->dodge_chance_mod);
        c->equipped_boots = nullptr;
    }
}

void weapon::use(player* c, int index)
{
    if (this->item_equipped == false && c->equipped_weapon != nullptr) return;

    bool was_equipped = this->is_equipped();
    armor::use(c, index);

    if (was_equipped == false && this->item_equipped == true)
    {

        add_player_damage(c, this->damage_stat);
        c->equipped_weapon = this;
    }
    else if (was_equipped == true && this->item_equipped == false)
    {
        this->reduce_player_damage(c, this->damage_stat);
        c->equipped_weapon = nullptr;
    }
}

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

void other_item::use(player* c, int index)
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
   
void food::use(player* c, int index)
{
    usable::use(c, index);
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
item* food::clone() const { return new food(*this); }
item* other_item::clone() const { return new other_item(*this); }