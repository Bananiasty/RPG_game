#include <random>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "raylib.h"
#include <utility>
#include <regex>



character::character(std::string n, int hp, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, int rdh, Texture2D g, Vector3 pos, float rot)
    : name(n), health(hp), base_defense(bdef), base_damage(bdmg), block_chance(b_ch), crit_chance(c_ch), dodge_chance(d_ch), reduced_head_damage(rdh), grafika(g)
{
    max_health = hp;
};

player::player(std::string n, int hp, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, int rdh, int xp, int level, Texture2D g, Vector3 pos, float rot)
    : character(n, hp, bdef, bdmg, b_ch, c_ch, d_ch, rdh, g, { 0.0f, 0.0f, 0.0f }, rot)
{
    bag = new inventory();
    equipped_items = new inventory();
    equipment = new inventory();
    inv_items = new inventory();
    food = new inventory();
    books = new inventory();
    this->level = 1;
    this->xp = 0;
    gold = 0;

    queued_animation_texture = nullptr;
    queued_frame_count = 0;
    queued_frame_time = 0.0;
};

enemy::enemy(const enemy_config& config): 
    id_number(config.id),
    character(
        config.name,
        config.hp,
        config.armor,
        config.damage,
        config.block_chance,
        config.crit_chance,
        config.dodge_chance,
        config.reduced_head_damage,
        config.texture,
        config.position,
        config.rotation
    ),
    
    difficulty(config.level),
    intro_text(config.description)
{
    this->position = config.position;

}

void player::set_name(const std::string& new_name)
{
    this->name = new_name;
};



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

bool character::is_dead()
{
    return health <= 0;
}

std::pair<int, bool> character::calculate_dmg()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(1, 100);
    int damage_amount = this->get_damage();
    bool is_crit = false;

    if (distr(gen) <= this->get_crit_chance())
    {
        damage_amount *= 2;
        is_crit = true;
    }

    return std::make_pair(damage_amount, is_crit);
}

int character::take_damage(int dmg_amount, const character* player_ptr, bool is_crit, bool is_guard)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(1, 100);

    bool czy_to_gracz = (this == player_ptr);
    
    if (distr(gen) <= this->get_dodge_chance())
    {
        gamestate::gameLogs.push_back(TextFormat("Unik!"));
        return 0;
    }

    int final_dmg = dmg_amount;
    bool is_blocked = false;
    
    if (czy_to_gracz==true && is_guard==true)
    {
        final_dmg /= 2;
        is_blocked = true;
    }
    
    int incoming_damage = std::max(0, final_dmg - this->get_defense());

    if (is_blocked)
    {
        if (czy_to_gracz)
            gamestate::gameLogs.push_back(TextFormat("Blok! Otrzymales %d obrazen", incoming_damage));
        else
            gamestate::gameLogs.push_back(TextFormat("Blok! Zadales %d obrazen", incoming_damage));
    }
    else if (is_crit)
    {
        if (czy_to_gracz)
            gamestate::gameLogs.push_back(TextFormat("Cios krytyczny! Otrzymales %d obrazen", incoming_damage));
        else
            gamestate::gameLogs.push_back(TextFormat("Cios krytyczny! Zadales %d obrazen", incoming_damage));
    }
    else
    {
        if (czy_to_gracz)
            gamestate::gameLogs.push_back(TextFormat("Otrzymales %d obrazen", incoming_damage)); 
        else
            gamestate::gameLogs.push_back(TextFormat("Zadales %d obrazen", incoming_damage)); 
    }
    
    this->health -= incoming_damage;
    return incoming_damage;
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


void player::take_all_loot(chest* c)
{
    for (item* i : c->chest_loot)
    {
        bag->add_item(i);
    }
    c->chest_loot.clear();
}

void player::take_item(chest* c, item* it)
{
    if (c == nullptr || it == nullptr)
    {
        return;
    }
    std::cout << "[TAKE_ITEM] Adres obiektu gracza: " << this
        << " | Adres torby: " << this->bag << std::endl;

    this->bag->add_item(it);
    std::cout << "[TAKE_ITEM] Przedmiotow w torbie: " << this->bag->items.size() << std::endl;

    auto it_chest = std::find(c->chest_loot.begin(), c->chest_loot.end(), it);
    if (it_chest != c->chest_loot.end())
    {
        c->chest_loot.erase(it_chest);
    }
    
    auto it_enemy = std::find(c->enemy_loot.begin(), c->enemy_loot.end(), it);
    if (it_enemy != c->enemy_loot.end())
    {
        c->enemy_loot.erase(it_enemy);
    }
    this->sort_bag();
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

