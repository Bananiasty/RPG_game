#include "entities/Enemy.h"
#include "raylib.h"
#include <vector>

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
