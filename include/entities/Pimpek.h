#pragma once
#include "entities/Enemy.h"

class pimpek : public enemy
{
public:
	pimpek(const enemy_config& config) : enemy(config)
	{
		limbs.left_leg.can_attack = true;
		limbs.left_leg.damage = 6;
		limbs.right_leg.can_attack = true;
		limbs.right_leg.damage = 6;
	}

	BodyPart calculate_hovered_body_part(Vector3 drawPos, float targetWidth, float targetHeight, Camera3D camera) override;
	int take_damage(int dmg_amount, const character* attacker, bool is_crit, bool is_guard, BodyPart hit_part, gamestate* gs, Vector3 impact_pos = { 0.0f, 0.0f, 0.0f }) override;

	SpriteRenderParams get_render_params(Texture2D texture) const override
	{
		constexpr float PIMPEK_Y_OFFSET = -0.40f;

		float frameWidth = (float)texture.width / 8.0f;
		float frameHeight = (float)texture.height / 4.0f;
		float targetHeight = 2.0f;
		float targetWidth = targetHeight * (frameWidth / frameHeight);

		Vector3 pos = get_position();
		Vector3 drawPos = { pos.x, pos.y + PIMPEK_Y_OFFSET, pos.z };

		return { drawPos, targetWidth, targetHeight, frameWidth, frameHeight };
	}

	pimpek* clone() const override
	{
		return new pimpek(*this);
	}

	std::vector<BodyPart> decide_attacks() override;
	int execute_ai_turn(character& target) override;
};
