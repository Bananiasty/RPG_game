#include "combat/StatusEffectFactory.h"
#include "textureManager.h"

std::shared_ptr<StatusEffect> StatusEffectFactory::create(EffectType type, int duration, int potency, Texture2D icon)
{
	switch (type)
	{
	case EffectType::BLEED:
		return create_bleed(duration, potency, icon);
	case EffectType::PLAGUE:
		return create_plague(duration, potency, 0.25f, icon);
	default:
		return nullptr;
	}
}

std::shared_ptr<BleedEffect> StatusEffectFactory::create_bleed(int duration, int damage_per_turn, Texture2D icon)
{
	if (icon.id == 0)
	{
		icon = textures.bleeding_icon;
	}
	return std::make_shared<BleedEffect>(duration, damage_per_turn, icon);
}

std::shared_ptr<PlagueEffect> StatusEffectFactory::create_plague(int duration, int damage_per_turn, float def_reduction, Texture2D icon)
{
	if (icon.id == 0)
	{
		icon = textures.plague_icon;
	}
	return std::make_shared<PlagueEffect>(duration, damage_per_turn, def_reduction, icon);
}
