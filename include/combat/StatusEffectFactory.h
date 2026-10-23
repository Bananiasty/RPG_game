#pragma once
#include <memory>
#include "combat/StatusEffect.h"
#include "combat/BleedEffect.h"
#include "combat/PlagueEffect.h"

class StatusEffectFactory
{
public:
	static std::shared_ptr<StatusEffect> create(EffectType type, int duration, int potency, Texture2D icon = { 0 });

	static std::shared_ptr<BleedEffect> create_bleed(int duration, int damage_per_turn, Texture2D icon = { 0 });

	static std::shared_ptr<PlagueEffect> create_plague(int duration, int damage_per_turn, float def_reduction = 0.25f, Texture2D icon = { 0 });
};
