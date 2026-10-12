#pragma once
#include <string>
#include "raylib.h"
#include "combat/Limb.h"

struct enemy_config
{
	int id = -1;
	std::string name = "";
	int max_health = 0;
	limbs_struct limbs;
	int armor = 0;
	int block_chance = 0;
	int crit_chance = 0;
	int dodge_chance = 0;
	int reduced_head_damage = 0;
	Texture2D texture = {};
	Vector3 position = { 0.0f, 1.0f, 0.0f };
	float rotation = 0.0f;
	int level = 1;
	std::string description = "";
};
