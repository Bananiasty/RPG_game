#include <iostream>
#include <ctime>
#include <cstdlib>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "raylib.h"




int battle::player_attack() {

	if (!p_ref.is_dead() && !e_ref.is_dead()) {
		auto [p_dmg, crit] = p_ref.calculate_dmg();
		int final_dmg = e_ref.take_damage(p_dmg, &p_ref, crit);

		return final_dmg;
	}
	return 0;
}

int battle::enemy_turn() {
	auto [e_dmg, crit] = e_ref.calculate_dmg();
	int final_dmg = p_ref.take_damage(e_dmg, &p_ref, crit);
	
	return final_dmg;
}
