#include <random>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "raylib.h"
#include <utility>
#include <regex>



character::character(std::string n, int h, int m, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, Texture2D g)
    : name(n), health(h), mana(m), base_defense(bdef), base_damage(bdmg), block_chance(b_ch), crit_chance(c_ch), dodge_chance(d_ch), grafika(g)
{
    max_health = h;
    max_mana = m;
};

player::player(std::string n, int h, int m, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, Texture2D g)
    : character(n, h, m, bdef, bdmg, b_ch, c_ch, d_ch, g)
{
    bag = new inventory();
    experience = 0;
    gold = 0;
};

enemy::enemy(std::string n, int h, int m, int bdef, int bdmg, int b_ch, int c_ch, int d_ch, Texture2D g, int dif, std::string intro)
    : character(n, h, m, bdef, bdmg, b_ch, c_ch, d_ch, g), difficulty(dif), intro_text(intro){};

void player::set_name(const std::string& new_name)
{
    this->name = new_name;
};




//REGEX
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

int character::take_damage(int dmg_amount, const character* player_ptr, bool is_crit)
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
    
    if (distr(gen) <= this->get_block_chance())
    {
        
        final_dmg /= 2;
        is_blocked = true;
        
    }
    
    int incoming_damage = final_dmg - this->get_defense();

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




void player::take_all_loot(chest* c)
{
    for (item* i : c->chest_loot)
    {
        bag->add_item(i);
    }
    c->chest_loot.clear();
}

enemy* enemy::clone() const
{
    return new enemy(*this);
}
