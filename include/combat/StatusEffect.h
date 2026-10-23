#pragma once
#include <string>
#include <memory>
#include "raylib.h"

class character;
class gamestate;

enum class EffectType
{
	NONE = 0,
	BLEED,
	PLAGUE,
	POISON,
	WEAKEN,
	STUN
};

class StatusEffect
{
protected:
	EffectType type;
	std::string name;
	int duration_turns;
	int potency;
	Texture2D icon;

public:
	StatusEffect(EffectType t, std::string n, int duration, int pot, Texture2D ic = { 0 })
		: type(t), name(std::move(n)), duration_turns(duration), potency(pot), icon(ic) {
	}

	virtual ~StatusEffect() = default;

	virtual std::shared_ptr<StatusEffect> clone() const = 0;

	// Cykl życia efektu
	virtual void on_apply(character& target, gamestate* gs = nullptr) {}
	virtual void on_turn_start(character& target, gamestate* gs = nullptr) {}
	virtual void on_turn_end(character& target, gamestate* gs = nullptr) {}
	virtual void on_remove(character& target, gamestate* gs = nullptr) {}

	// Modyfikatory parametrów postaci
	virtual int modify_incoming_damage(int raw_dmg) const { return raw_dmg; }
	virtual int modify_outgoing_damage(int raw_dmg) const { return raw_dmg; }
	virtual int modify_defense(int raw_def) const { return raw_def; }

	// Strategia stackowania
	virtual bool try_stack(const StatusEffect& other)
	{
		return false;
	}

	// Metoda szablonowa odliczania tury
	virtual bool tick()
	{
		return (--duration_turns <= 0);
	}

	// Gettery i Settery
	EffectType get_type() const { return type; }
	const std::string& get_name() const { return name; }
	int get_remaining_turns() const { return duration_turns; }
	void set_remaining_turns(int turns) { duration_turns = turns; }
	int get_potency() const { return potency; }
	void set_potency(int val) { potency = val; }
	Texture2D get_icon() const { return icon; }
	void set_icon(Texture2D ic) { icon = ic; }
	bool is_expired() const { return duration_turns <= 0; }
};
