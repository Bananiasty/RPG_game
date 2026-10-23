#include "combat/PlagueEffect.h"
#include "entities/Character.h"
#include "entities/Player.h"
#include "gamestates.h"
#include <algorithm>

PlagueEffect::PlagueEffect(int duration, int damage_per_turn, float def_reduction, Texture2D ic)
	: StatusEffect(EffectType::PLAGUE, "Plague", duration, damage_per_turn, ic),
	  defense_reduction_ratio(def_reduction)
{
}

std::shared_ptr<StatusEffect> PlagueEffect::clone() const
{
	return std::make_shared<PlagueEffect>(*this);
}

void PlagueEffect::on_apply(character& target, gamestate* gs)
{
	bool is_player = (dynamic_cast<const player*>(&target) != nullptr);
	if (is_player)
	{
		gamestate::gameLogs.push_back(TextFormat("You caught the Plague! Defense reduced by %d%%!", static_cast<int>(defense_reduction_ratio * 100)));
	}
	else
	{
		gamestate::gameLogs.push_back(TextFormat("%s caught the Plague!", target.get_name().c_str()));
	}
}

void PlagueEffect::on_turn_start(character& target, gamestate* gs)
{
	if (this->potency <= 0) return;

	target.take_status_damage(this->potency, gs, "Plague", DARKGREEN);
}

void PlagueEffect::on_remove(character& target, gamestate* gs)
{
	bool is_player = (dynamic_cast<const player*>(&target) != nullptr);
	if (is_player)
	{
		gamestate::gameLogs.push_back("The plague has subsided. Defense restored.");
	}
	else
	{
		gamestate::gameLogs.push_back(TextFormat("%s recovered from the Plague.", target.get_name().c_str()));
	}
}

int PlagueEffect::modify_defense(int raw_def) const
{
	int reduced = static_cast<int>(raw_def * (1.0f - defense_reduction_ratio));
	return std::max(0, reduced);
}

bool PlagueEffect::try_stack(const StatusEffect& other)
{
	// Niezależne instancje: każda infekcja zarazy ma własny czas trwania i własne obrażenia
	return false;
}
