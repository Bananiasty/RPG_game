#pragma once
#include "combat/StatusEffect.h"

class PlagueEffect : public StatusEffect
{
private:
	float defense_reduction_ratio; // np. 0.25f = obniżenie obrony o 25%

public:
	PlagueEffect(int duration = 3, int damage_per_turn = 4, float def_reduction = 0.25f, Texture2D ic = { 0 });

	std::shared_ptr<StatusEffect> clone() const override;

	void on_apply(character& target, gamestate* gs = nullptr) override;
	void on_turn_start(character& target, gamestate* gs = nullptr) override;
	void on_remove(character& target, gamestate* gs = nullptr) override;

	int modify_defense(int raw_def) const override;
	bool try_stack(const StatusEffect& other) override;

	float get_defense_reduction_ratio() const { return defense_reduction_ratio; }
};
