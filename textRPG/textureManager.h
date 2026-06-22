#pragma once
#include "raylib.h"

struct textureManager {

	Texture2D kopalnia;

	Texture2D player;
	Texture2D goblin;
	Texture2D skeleton;
	Texture2D troll;
	Texture2D bandits;
	Texture2D guard;
	Texture2D dragon;
};

extern textureManager textures;
void LoadGameTextures();
 