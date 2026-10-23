#pragma once
#include "entities/Enemy.h"

class ghoul : public enemy
{
public:
	ghoul(const enemy_config& config) : enemy(config)
	{
		limbs.left_arm.is_intact = false;
		limbs.left_arm.hp = 0;

		limbs.right_arm.can_attack = true;
		limbs.right_arm.damage = 25;

		limbs.head.can_attack = true;
		limbs.head.applies_bleed = true;
		limbs.head.bleed_timer = 2;
		limbs.head.bleed_dmg = 5;
		limbs.head.damage = 7;
	}

	BodyPart calculate_hovered_body_part(Vector3 drawPos, float targetWidth, float targetHeight, Camera3D camera) override;

	SpriteRenderParams get_render_params(Texture2D texture) const override
	{
		constexpr float GHOUL_Y_OFFSET = 0.15f;

		float frameWidth = (float)texture.width / 8.0f;
		float frameHeight = (float)texture.height / 4.0f;
		float targetHeight = 2.0f;
		float targetWidth = targetHeight * (frameWidth / frameHeight);

		Vector3 pos = get_position();
		Vector3 drawPos = { pos.x, pos.y + GHOUL_Y_OFFSET, pos.z };

		return { drawPos, targetWidth, targetHeight, frameWidth, frameHeight };
	}

	ghoul* clone() const override
	{
		return new ghoul(*this);
	}

	std::vector<BodyPart> decide_attacks() override;
	int execute_ai_turn(character& target) override;
};
