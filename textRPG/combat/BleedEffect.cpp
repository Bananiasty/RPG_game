#include "combat/BleedEffect.h"
#include "entities/Character.h"
#include "entities/Player.h"
#include "gamestates.h"

BleedEffect::BleedEffect(int duration, int damage_per_turn, Texture2D ic)
	: StatusEffect(EffectType::BLEED, "Bleeding", duration, damage_per_turn, ic)
{
}

std::shared_ptr<StatusEffect> BleedEffect::clone() const
{
	return std::make_shared<BleedEffect>(*this);
}

void BleedEffect::on_apply(character& target, gamestate* gs)
{
	bool is_player = (dynamic_cast<const player*>(&target) != nullptr);
	if (is_player)
	{
		gamestate::gameLogs.push_back("You started bleeding!");
	}
	else
	{
		gamestate::gameLogs.push_back(TextFormat("%s started bleeding!", target.get_name().c_str()));
	}
}

void BleedEffect::on_turn_start(character& target, gamestate* gs)
{
	if (this->potency <= 0) return;

	target.take_status_damage(this->potency, gs, "Bleed", RED);
}

void BleedEffect::on_remove(character& target, gamestate* gs)
{
	bool is_player = (dynamic_cast<const player*>(&target) != nullptr);
	if (is_player)
	{
		gamestate::gameLogs.push_back("Your bleeding has stopped.");
	}
	else
	{
		gamestate::gameLogs.push_back(TextFormat("%s stopped bleeding.", target.get_name().c_str()));
	}
}

bool BleedEffect::try_stack(const StatusEffect& other)
{
	// Niezależne instancje: każda rana ma własny czas trwania i własne obrażenia
	return false;
}
