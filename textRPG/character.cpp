#include <random>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "raylib.h"
#include "raymath.h"
#include <utility>
#include <regex>



character::character(std::string n, const limbs_struct& l, int bdef, int b_ch, int c_ch, int d_ch, int rdh, Texture2D g, Vector3 pos, float rot)
    : name(n), limbs(l), base_defense(bdef), block_chance(b_ch), crit_chance(c_ch), dodge_chance(d_ch), reduced_head_damage(rdh), grafika(g)
{
    this->position = pos;
    this->rotation = rot;

    this->recalculate_max_health();
    this->current_health = this->max_health;
};

player::player(std::string n, const limbs_struct& l, int bdef, int b_ch, int c_ch, int d_ch, int rdh, int xp, int level, Texture2D g, Vector3 pos, float rot)
    : character(n, l, bdef, b_ch, c_ch, d_ch, rdh, g, pos, rot)
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
    queued_frame_time = 0.0;
};

enemy::enemy(const enemy_config& config)
    : character(
        config.name,
		config.limbs,
        config.armor,
        config.block_chance,
        config.crit_chance,
        config.dodge_chance,
        config.reduced_head_damage,
        config.texture,
        config.position,
        config.rotation
    ),
    id_number(config.id),
    difficulty(config.level),
    intro_text(config.description)
{
    this->position = config.position;
}

dead_body::dead_body(enemy* e, const Model* model, int slots_count, std::vector<std::unique_ptr<item>> loot) : drop_object(e ? e->get_position() : Vector3{ 0, 0, 0 }, model, slots_count, std::move(loot)), enemy_ptr(e){}


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

bool player::is_dead()
{
    if (!limbs.torso.is_intact)
    {
        return true;
    }
    if (!limbs.head.is_intact)
    {
        return true;
    }
    if (current_health <= 0)
    {
        return true;
    }

    return false;
}
bool enemy::is_dead()
{
    if (!limbs.torso.is_intact)
    {
        return true;
    }
    if (!limbs.head.is_intact && !can_survive_without_head)
    {
        return true;
    }
    if(current_health<=0)
    {
        return true;
    }

    return false;
}

std::pair<int, bool> character::calculate_dmg(int limb_damage)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(1, 100);
    int damage_amount = limb_damage;
    bool is_crit = false;

    if (distr(gen) <= this->get_crit_chance())
    {
        damage_amount *= 2;
        is_crit = true;
    }

    return std::make_pair(damage_amount, is_crit);
}

int character::take_damage(int dmg_amount, const character* attacker, bool is_crit, bool is_guard, BodyPart hit_part, gamestate* gs, Vector3 impact_pos)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(1, 100);
    static std::uniform_real_distribution<float> jitter(-0.05f, 0.05f);

    bool gracz_take_dmg = (dynamic_cast<const player*>(this) != nullptr);

    Vector3 text_pos;

    if (impact_pos.x != 0.0f || impact_pos.y != 0.0f || impact_pos.z != 0.0f)
    {
        text_pos = impact_pos;
    }
    else
    {
        text_pos = this->get_visual_position();

        switch (hit_part)
        {
        case BodyPart::HEAD:
            text_pos.y += 1.8f;
            break;
        case BodyPart::TORSO:
            text_pos.y += 1.2f;
            break;
        case BodyPart::LEFT_ARM:
        case BodyPart::RIGHT_ARM:
            text_pos.y += 1.2f;
            break;
        case BodyPart::LEFT_LEG:
        case BodyPart::RIGHT_LEG:
            text_pos.y += 0.5f;
            break;
        default:
            text_pos.y += 1.2f;
            break;
        }
    }

    text_pos.y += 0.25f;
    text_pos.x += jitter(gen);
    text_pos.z += jitter(gen);

    if (distr(gen) <= this->get_dodge_chance())
    {
        gamestate::gameLogs.push_back(TextFormat("Missed!"));
        if (gs)
        {
            gs->spawn_floating_text(text_pos, "Missed!", false);
        }
        return 0;
    }

    int final_dmg = dmg_amount;
    bool is_blocked = false;

    if (gracz_take_dmg && is_guard)
    {
        final_dmg /= 2;
        is_blocked = true;
    }

    int incoming_damage = std::max(0, final_dmg - this->get_defense());

    if (is_blocked)
    {
        if (gracz_take_dmg)
            gamestate::gameLogs.push_back(TextFormat("Block! Received %d damage", incoming_damage));
        else
            gamestate::gameLogs.push_back(TextFormat("Block! Dealt %d damage", incoming_damage));
    }
    else if (is_crit)
    {
        if (gracz_take_dmg)
            gamestate::gameLogs.push_back(TextFormat("Crit! Received %d damage", incoming_damage));
        else
            gamestate::gameLogs.push_back(TextFormat("Critical Hit! Dealt %d damage", incoming_damage));
    }
    else
    {
        if (gracz_take_dmg)
            gamestate::gameLogs.push_back(TextFormat("Received %d damage", incoming_damage));
        else
            gamestate::gameLogs.push_back(TextFormat("Dealt %d damage", incoming_damage));
    }

    if (gs)
    {
        std::string floating_str = std::to_string(incoming_damage);
        gs->spawn_floating_text(text_pos, floating_str, is_crit);
    }

    auto& target_limb = this->limbs.get_limb(hit_part);
    int actual_limb_damage = std::min(incoming_damage, target_limb.hp);

    target_limb.take_damage(actual_limb_damage);
    this->current_health = std::max(0, this->current_health - actual_limb_damage);

    if (!target_limb.is_intact)
    {
        this->recalculate_max_health();
        this->current_health = std::min(this->current_health, this->max_health);
    }

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


void player::take_all_loot(drop_object* o)
{
    if (o == nullptr) return;

    for (const auto& i : o->drop_loot)
    {
        bag->add_item(i.get());
    }
    o->drop_loot.clear();
}

void player::take_item(drop_object* o, item* it)
{
    if (o == nullptr || it == nullptr)
    {
        return;
    }

    this->bag->add_item(it);

    auto loot_container = std::find_if(o->drop_loot.begin(), o->drop_loot.end(), [it](const std::unique_ptr<item>& ptr) 
        {
            return ptr.get() == it;
        }
    );

    if (loot_container != o->drop_loot.end())
    {
        o->drop_loot.erase(loot_container);
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

void character::recalculate_max_health()
{
    int max_hp = this->limbs.get_total_max_hp();
    int max_hp_calc = (int)(max_hp * 0.8);
	this->set_max_health(max_hp_calc);
}

