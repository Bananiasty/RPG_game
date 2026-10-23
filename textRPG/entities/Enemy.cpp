#include "entities/Enemy.h"
#include "combat/StatusEffectFactory.h"
#include "raylib.h"
#include "gamestates.h"
#include "AudioManager.h"
#include <vector>

enemy::enemy(const enemy_config& config)
    : character(
        config.name,
        config.limbs,
        config.armor,
        config.block_chance,
        config.crit_chance,
        config.dodge_chance,
        config.movement_speed,
        config.texture,
        config.position,
        config.rotation
    ),
    id_number(config.id),
    difficulty(config.level),
    intro_text(config.description)
{
    this->position = config.position;
    if (config.max_health > 0)
    {
        this->set_max_health(config.max_health);
        this->current_health = this->max_health;
    }
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
    if (current_health <= 0)
    {
        return true;
    }

    return false;
}

BodyPart enemy::select_random_target_part(const character& target)
{
    std::vector<BodyPart> valid_parts;

    if (target.limbs.torso.is_intact)     valid_parts.push_back(BodyPart::TORSO);
    if (target.limbs.head.is_intact)      valid_parts.push_back(BodyPart::HEAD);
    if (target.limbs.left_arm.is_intact)  valid_parts.push_back(BodyPart::LEFT_ARM);
    if (target.limbs.right_arm.is_intact) valid_parts.push_back(BodyPart::RIGHT_ARM);
    if (target.limbs.left_leg.is_intact)  valid_parts.push_back(BodyPart::LEFT_LEG);
    if (target.limbs.right_leg.is_intact) valid_parts.push_back(BodyPart::RIGHT_LEG);

    if (valid_parts.empty())
    {
        return BodyPart::TORSO;
    }

    int randomIndex = GetRandomValue(0, (int)valid_parts.size() - 1);
    return valid_parts[randomIndex];
}

int enemy::GetAdjustedSideLimit(int baseSideLimit, int frameIndex)
{
    if (baseSideLimit == 0)
    {
        return 0;
    }

    bool isBackView = (frameIndex >= 2 && frameIndex <= 6);
    if (isBackView)
    {
        return (baseSideLimit == 1) ? 2 : 1;
    }

    return baseSideLimit;
}

int enemy::perform_strike(BodyPart attacking_part, character& target)
{
    if (target.is_dead()) return 0;

    const auto& attacking_limb = this->limbs.get_limb(attacking_part);
    if (!attacking_limb.is_intact || !attacking_limb.can_attack) return 0;

    if (attacking_limb.applies_bleed)
    {
        target.apply_effect(StatusEffectFactory::create_bleed(attacking_limb.bleed_timer, attacking_limb.bleed_dmg), this->gs);
    }

    if (attacking_limb.applies_plague)
    {
        target.apply_effect(StatusEffectFactory::create_plague(attacking_limb.plague_timer, attacking_limb.plague_dmg), this->gs);
    }

    BodyPart hit_part = this->select_random_target_part(target);
    auto [e_dmg, crit] = this->calculate_dmg(attacking_limb.damage);
    audio.play_sound(SoundID::ENEMY_HIT);

    int final_dmg = target.take_damage(e_dmg, this, crit, target.is_guard, hit_part, this->gs);
    return final_dmg;
}
