#pragma once
#include "combat/StatusEffect.h"

class BleedEffect : public StatusEffect
{
public:
	BleedEffect(int duration = 2, int damage_per_turn = 5, Texture2D ic = { 0 });

	std::shared_ptr<StatusEffect> clone() const override;

	void on_apply(character& target, gamestate* gs = nullptr) override;
	void on_turn_start(character& target, gamestate* gs = nullptr) override;
	void on_remove(character& target, gamestate* gs = nullptr) override;

	bool try_stack(const StatusEffect& other) override;
};
