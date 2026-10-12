#include "entities/Player.h"
#include "inventory/Item.h"
#include "inventory/Equipment.h"
#include "inventory/Consumable.h"
#include "inventory/Scroll.h"
#include "struct.h"
#include "textureManager.h"
#include <regex>
#include <algorithm>

player::player(std::string n, int hp, int bdef, int b_ch, int c_ch, int d_ch, int rdh, int xp, int level, Texture2D g, Vector3 pos, float rot)
    : character(n, hp, bdef, b_ch, c_ch, d_ch, rdh, textures.player, pos, rot)
{
    bag = new inventory();
    equipped_items = new inventory();
    equipment = new inventory();
    inv_items = new inventory();
    food = new inventory();
    books = new inventory();
    this->level = level;
    this->xp = xp;
    gold = 0;

    limbs.left_arm.can_attack = true;
    limbs.left_arm.damage = 10;

    limbs.right_arm.can_attack = true;
    limbs.right_arm.damage = 10;

    queued_animation_texture = nullptr;
    queued_frame_count = 0;
    queued_frame_time = 0.0f;
}

player::~player()
{
    if (bag != nullptr)
    {
        for (item* it : bag->items)
        {
            delete it;
        }
        delete bag;
        bag = nullptr;
    }
    delete equipped_items;
    equipped_items = nullptr;
    delete equipment;
    equipment = nullptr;
    delete inv_items;
    inv_items = nullptr;
    delete food;
    food = nullptr;
    delete books;
    books = nullptr;
}

void player::set_name(const std::string& new_name)
{
    this->name = new_name;
}

bool player::validate_and_set_name(const std::string& new_name)
{
    std::regex nick_pattern("^[a-zA-Z0-9]{3,12}$");

    if (std::regex_match(new_name, nick_pattern))
    {
        this->set_name(new_name);
        return true;
    }
    return false;
}

bool player::is_dead()
{
    return current_health <= 0;
}

void player::player_guard()
{
    if (this->is_guard == false)
    {
        this->is_guard = true;
    }
}

void player::player_attack()
{
    if (this->is_attack == false)
    {
        this->is_attack = true;
    }
}

void player::take_all_loot(object* o)
{
    if (o == nullptr) return;
    loot_object* drop = dynamic_cast<loot_object*>(o);
    if (drop == nullptr) return;

    for (auto& up : drop->drop_loot)
    {
        if (up)
        {
            bag->add_item(up.release());
        }
    }
    drop->drop_loot.clear();
}

void player::grant_xp()
{
    int xp_grant = this->xp_from_enemy_dif;
    this->xp += xp_grant;
}

void player::check_level_up()
{
    if (xp >= xp_to_level_up)
    {
        this->level += 1;
        int rest_xp = xp - xp_to_level_up;
        xp_to_level_up = this->level * 2;
        this->xp = rest_xp;
    }
}

void player::sort_bag()
{
    if (equipment != nullptr) equipment->items.clear();
    if (inv_items != nullptr) inv_items->items.clear();
    if (food != nullptr) food->items.clear();
    if (books != nullptr) books->items.clear();

    if (bag == nullptr || bag->items.empty())
    {
        return;
    }

    for (item* it : bag->items)
    {
        if (it == nullptr) continue;

        switch (it->get_type())
        {
        case item_type::ARMOR:
        {
            equipment->add_item(it);
            break;
        }
        case item_type::ITEM:
        {
            inv_items->add_item(it);
            break;
        }
        case item_type::FOOD:
        {
            food->add_item(it);
            break;
        }
        case item_type::BOOK:
        {
            books->add_item(it);
            break;
        }
        default:
            break;
        }
    }
}

void player::drop_item(item* item_to_drop)
{
    if (this->bag == nullptr || item_to_drop == nullptr)
    {
        return;
    }
    std::erase(this->bag->items, item_to_drop);
}
