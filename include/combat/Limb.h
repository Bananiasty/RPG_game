#pragma once
#include "combat/BodyPart.h"

struct limb
{
	int hp = 1;
	int max_hp = 1;
	bool is_intact = true;
	bool can_attack = false;
	int damage = 0;

	bool applies_bleed = false;
	int bleed_dmg = 0;
	int bleed_timer = 0;

	bool limb_wounded = false;
	bool severely_wounded = false;

	limb(int max_val = 1, bool can_atk = false, int dmg = 0)
		: hp(max_val), max_hp(max_val), is_intact(true), can_attack(can_atk), damage(dmg),
		  limb_wounded(false), severely_wounded(false) {
	}

	bool take_damage(int amount)
	{
		if (!is_intact) return false;

		hp -= amount;
		if (hp <= 0)
		{
			hp = 0;
			is_intact = false;
			return true;
		}
		return false;
	}
};

struct limbs_struct
{
	limb head = 1;
	limb torso = 1;
	limb left_arm = 1;
	limb right_arm = 1;
	limb left_leg = 1;
	limb right_leg = 1;

	limbs_struct() = default;
	limbs_struct(int h, int t, int la, int ra, int ll, int rl)
		: head(h), torso(t), left_arm(la), right_arm(ra), left_leg(ll), right_leg(rl) {
	}

	limb& get_limb(BodyPart part)
	{
		switch (part)
		{
		case BodyPart::HEAD:
			return head;
		case BodyPart::TORSO:
			return torso;
		case BodyPart::LEFT_ARM:
			return left_arm;
		case BodyPart::RIGHT_ARM:
			return right_arm;
		case BodyPart::LEFT_LEG:
			return left_leg;
		case BodyPart::RIGHT_LEG:
			return right_leg;
		default:
			return torso;
		}
	}

	const limb& get_limb(BodyPart part) const
	{
		switch (part)
		{
		case BodyPart::HEAD:
			return head;
		case BodyPart::TORSO:
			return torso;
		case BodyPart::LEFT_ARM:
			return left_arm;
		case BodyPart::RIGHT_ARM:
			return right_arm;
		case BodyPart::LEFT_LEG:
			return left_leg;
		case BodyPart::RIGHT_LEG:
			return right_leg;
		default:
			return torso;
		}
	}
};
