#include <random>
#include <algorithm>
#include <utility>
#include "entities/Character.h"
#include "entities/Player.h"
#include "inventory/Equipment.h"
#include "gamestates.h"
#include "raymath.h"

character::character(std::string n, int hp, int bdef, int b_ch, int c_ch, int d_ch, float m_s, Texture2D g, Vector3 pos, float rot)
    : name(n), max_health(hp), current_health(hp), base_defense(bdef), block_chance(b_ch), crit_chance(c_ch), dodge_chance(d_ch), movement_speed(m_s), grafika(g)
{
    this->position = pos;
    this->rotation = rot;
}

character::character(std::string n, const limbs_struct& l, int bdef, int b_ch, int c_ch, int d_ch, float m_s, Texture2D g, Vector3 pos, float rot)
    : name(n), limbs(l), base_defense(bdef), block_chance(b_ch), crit_chance(c_ch), dodge_chance(d_ch), movement_speed(m_s), grafika(g)
{
    this->position = pos;
    this->rotation = rot;
    this->recalculate_max_health();
    this->current_health = this->max_health;
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

void character::process_turn_start_effects(gamestate* gs)
{
    if (this->current_health <= 0) return;

    bool gracz = (dynamic_cast<const player*>(this) != nullptr);

    if (this->is_bleeding)
    {
        int bleed_damage = this->applied_bleed_damage;

        if (bleed_damage > 0)
        {
            this->current_health = std::max(0, this->current_health - bleed_damage);

            if (gracz)
            {
                gamestate::gameLogs.push_back(TextFormat("You're bleeding! Received %d damage", bleed_damage));
            }
            else
            {
                gamestate::gameLogs.push_back(TextFormat("Enemy bleeds! Dealt %d damage", bleed_damage));
            }

            if (gs)
            {
                Vector3 text_pos = this->get_visual_position();
                text_pos.y += 1.5f;
                gs->spawn_floating_text(text_pos, TextFormat("-%d Bleed", bleed_damage), false, true);
            }
        }

        this->bleed_status_timer--;
        if (this->bleed_status_timer <= 0)
        {
            this->is_bleeding = false;
            this->applied_bleed_damage = 0;
            this->bleed_status_timer = 0;
        }
    }
}

int character::take_damage(int dmg_amount, const character* attacker, bool is_crit, bool is_guard, BodyPart hit_part, gamestate* gs, Vector3 impact_pos)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(1, 100);
    static std::uniform_real_distribution<float> jitter(-0.05f, 0.05f);

    bool gracz_take_dmg = (dynamic_cast<const player*>(this) != nullptr);

    // POZYCJA TEKSTU
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
        case BodyPart::HEAD:      text_pos.y += 1.8f; break;
        case BodyPart::TORSO:     text_pos.y += 1.2f; break;
        case BodyPart::LEFT_ARM:  text_pos.y += 1.3f; text_pos.x -= 0.4f; break;
        case BodyPart::RIGHT_ARM: text_pos.y += 1.3f; text_pos.x += 0.4f; break;
        case BodyPart::LEFT_LEG:  text_pos.y += 0.5f; text_pos.x -= 0.3f; break;
        case BodyPart::RIGHT_LEG: text_pos.y += 0.5f; text_pos.x += 0.3f; break;
        default:                  text_pos.y += 1.0f; break;
        }
    }

    text_pos.x += jitter(gen);
    text_pos.y += jitter(gen);
    text_pos.z += jitter(gen);

    // DODGE CHECK
    if (distr(gen) <= this->get_dodge_chance())
    {
        if (gs)
        {
            gs->spawn_floating_text(text_pos, "DODGE", false, false);
        }
        return 0;
    }

    // BLOCK CHECK
    if (distr(gen) <= this->get_block_chance())
    {
        if (gs)
        {
            gs->spawn_floating_text(text_pos, "BLOCK", false, false);
        }
        return 0;
    }

    // CALCULATE INCOMING DAMAGE
    int incoming_damage = dmg_amount;



    if (is_guard)
    {
        incoming_damage /= 2;
    }

    incoming_damage = std::max(0, incoming_damage - this->get_defense());

    // SPAWN DAMAGE TEXT
    if (gs)
    {
        gs->spawn_floating_text(text_pos, TextFormat("%d", incoming_damage), is_crit, false);
    }

    if (gracz_take_dmg)
    {
        // Enemy attacking Player: deals straight up body damage to player's overall HP
        this->current_health = std::max(0, this->current_health - incoming_damage);

        if (attacker != nullptr && dynamic_cast<const player*>(attacker) == nullptr)
        {
            // Possibility to wound one of player's limbs
            BodyPart chosen_part = hit_part;
            if (chosen_part == BodyPart::NONE)
            {
                static const BodyPart all_parts[] = {
                    BodyPart::TORSO, BodyPart::HEAD, BodyPart::LEFT_ARM,
                    BodyPart::RIGHT_ARM, BodyPart::LEFT_LEG, BodyPart::RIGHT_LEG
                };
                std::uniform_int_distribution<> p_distr(0, 5);
                chosen_part = all_parts[p_distr(gen)];
            }

            auto& target_limb = this->limbs.get_limb(chosen_part);
            const char* part_name = "body";
            switch (chosen_part)
            {
            case BodyPart::HEAD:      part_name = "head"; break;
            case BodyPart::TORSO:     part_name = "torso"; break;
            case BodyPart::LEFT_ARM:  part_name = "left arm"; break;
            case BodyPart::RIGHT_ARM: part_name = "right arm"; break;
            case BodyPart::LEFT_LEG:  part_name = "left leg"; break;
            case BodyPart::RIGHT_LEG: part_name = "right leg"; break;
            default: break;
            }

            int wound_chance = is_crit ? 40 : 25;
            if (distr(gen) <= wound_chance)
            {
                player* p = dynamic_cast<player*>(this);

                if (!target_limb.limb_wounded)
                {
                    target_limb.limb_wounded = true;
                    gamestate::gameLogs.push_back(TextFormat("Your %s was wounded!", part_name));
                    if (gs)
                    {
                        Vector3 w_pos = text_pos;
                        w_pos.y += 0.35f;
                        gs->spawn_floating_text(w_pos, TextFormat("%s wounded!", part_name), false, false);
                    }
                }
                else if (!target_limb.severely_wounded)
                {
                    // If enemy wounds this limb again, low chance of becoming severely wounded
                    int severe_chance = is_crit ? 40 : 25;
                    if (distr(gen) <= severe_chance)
                    {
                        target_limb.severely_wounded = true;
                        gamestate::gameLogs.push_back(TextFormat("Your %s was SEVERELY wounded!", part_name));
                        if (gs)
                        {
                            Vector3 w_pos = text_pos;
                            w_pos.y += 0.35f;
                            gs->spawn_floating_text(w_pos, TextFormat("%s severely wounded!", part_name), true, false);
                        }

                        // Player can no longer hold items in this limb
                        if (p != nullptr)
                        {
                            if (chosen_part == BodyPart::RIGHT_ARM && p->equipped_weapon != nullptr)
                            {
                                gamestate::gameLogs.push_back("Your right arm is severely wounded! You dropped your weapon.");
                                p->equipped_weapon->use(p);
                                p->sort_bag();
                            }
                            else if (chosen_part == BodyPart::LEFT_ARM && p->equipped_shield != nullptr)
                            {
                                gamestate::gameLogs.push_back("Your left arm is severely wounded! You dropped your shield.");
                                p->equipped_shield->use(p);
                                p->sort_bag();
                            }
                        }
                    }
                    else
                    {
                        gamestate::gameLogs.push_back(TextFormat("Your wounded %s took another hit!", part_name));
                    }
                }
            }
        }
    }
    else
    {
        // Player attacking Enemy
        const char* part_name = "body";
        switch (hit_part)
        {
        case BodyPart::HEAD:      part_name = "head"; break;
        case BodyPart::TORSO:     part_name = "body"; break;
        case BodyPart::LEFT_ARM:  part_name = "left arm"; break;
        case BodyPart::RIGHT_ARM: part_name = "right arm"; break;
        case BodyPart::LEFT_LEG:  part_name = "left leg"; break;
        case BodyPart::RIGHT_LEG: part_name = "right leg"; break;
        default: break;
        }

        if (hit_part == BodyPart::LEFT_ARM || hit_part == BodyPart::RIGHT_ARM ||
            hit_part == BodyPart::LEFT_LEG || hit_part == BodyPart::RIGHT_LEG)
        {
            // Enemy legs and arms do NOT count as total HP damage!
            auto& target_limb = this->limbs.get_limb(hit_part);
            target_limb.take_damage(incoming_damage);

            // If player destroys limbs, they disappear and can no longer be used
            if (!target_limb.is_intact)
            {
                target_limb.can_attack = false;
            }
        }
        else if (hit_part == BodyPart::TORSO || hit_part == BodyPart::HEAD)
        {
            // Head or body damage reduces total HP; if head or body is destroyed, enemy dies
            auto& target_limb = this->limbs.get_limb(hit_part);
            target_limb.take_damage(incoming_damage);
            this->current_health = std::max(0, this->current_health - incoming_damage);

            if (!target_limb.is_intact || this->current_health <= 0)
            {
                target_limb.is_intact = false;
                this->current_health = 0;
            }
        }
        else
        {
            // NONE (e.g. general / spell damage)
            this->current_health = std::max(0, this->current_health - incoming_damage);
        }
    }

    return incoming_damage;
}

void character::recalculate_max_health()
{
    int max_hp = this->limbs.torso.max_hp;
    if (dynamic_cast<player*>(this) != nullptr)
    {
        max_hp = this->limbs.torso.max_hp + this->limbs.head.max_hp;
    }
    this->set_max_health(max_hp);
}
