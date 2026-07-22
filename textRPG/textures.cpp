#include "TextureManager.h"
#include <iostream>

fx_animation global_fx;
textureManager textures;
ModelManager objects;

void LoadGameTextures()
{
	textures.kopalnia = LoadTexture("graphics/kopalnia.png");

	textures.player = LoadTexture("graphics/bohater.png");
	SetTextureFilter(textures.player, TEXTURE_FILTER_BILINEAR);

	textures.goblin = LoadTexture("graphics/goblin.png");
	SetTextureFilter(textures.goblin, TEXTURE_FILTER_BILINEAR);

	textures.skeleton = LoadTexture("graphics/szkielet.png");
	SetTextureFilter(textures.skeleton, TEXTURE_FILTER_BILINEAR);

	textures.troll = LoadTexture("graphics/troll.png");
	SetTextureFilter(textures.troll, TEXTURE_FILTER_BILINEAR);

	textures.bandits = LoadTexture("graphics/bandyci.png");
	SetTextureFilter(textures.bandits, TEXTURE_FILTER_BILINEAR);

	textures.guard = LoadTexture("graphics/straznik.png");
	SetTextureFilter(textures.guard, TEXTURE_FILTER_BILINEAR);

	textures.dragon = LoadTexture("graphics/smok.png");
	SetTextureFilter(textures.dragon, TEXTURE_FILTER_BILINEAR);

	textures.chest_t = LoadTexture("graphics/chest.png");
	SetTextureFilter(textures.dragon, TEXTURE_FILTER_BILINEAR);

	textures.lightning = LoadTexture("graphics/GAME ASSETS/SPELLS/1 Lightning/Lightning.png");
	textures.lightning_bolt = LoadTexture("graphics/GAME ASSETS/SPELLS/2 Lightning bolt/Lightning-bolt.png");
	textures.sun_strike = LoadTexture("graphics/GAME ASSETS/SPELLS/4 Sun strike/Sun-strike.png");
	textures.explosion = LoadTexture("graphics/GAME ASSETS/SPELLS/5 Explosion/Explosion.png");
	textures.spikes = LoadTexture("graphics/GAME ASSETS/SPELLS/6 Spikes/Spikes.png");
	textures.fireball = LoadTexture("graphics/GAME ASSETS/SPELLS/10 Fire ball/Fire-ball.png");
	textures.ignite = LoadTexture("graphics/GAME ASSETS/SPELLS/7 Fire wall/Fire-wall.png");

	textures.lightning_icon = LoadTexture("graphics/GAME ASSETS/SPELLS/Icons/1-Lightning.png");
	textures.lightning_bolt_icon = LoadTexture("graphics/GAME ASSETS/SPELLS/Icons/2-Lightning-bolt.png");
	textures.sun_strike_icon = LoadTexture("graphics/GAME ASSETS/SPELLS/Icons/4-Sun-strike.png");
	textures.explosion_icon = LoadTexture("graphics/GAME ASSETS/SPELLS/Icons/5-Explosion.png");
	textures.spikes_icon = LoadTexture("graphics/GAME ASSETS/SPELLS/Icons/6-Spikes.png");
	textures.fireball_icon = LoadTexture("graphics/GAME ASSETS/SPELLS/Icons/10-Fire-ball.png");
	textures.ignite_icon = LoadTexture("graphics/GAME ASSETS/SPELLS/Icons/7-Fire-wall.png");


	textures.short_sword = LoadTexture("graphics/GAME ASSETS/WEAPONS/Icons/item4.png");
	textures.long_sword = LoadTexture("graphics/GAME ASSETS/WEAPONS/Icons/Iicon_32_31.png");

	textures.leather_helm = LoadTexture("graphics/GAME ASSETS/ARMOR/item201.png");
	textures.iron_helm = LoadTexture("graphics/GAME ASSETS/ARMOR/item202.png");

	textures.leather_vest = LoadTexture("graphics/GAME ASSETS/ARMOR/item221.png");
	textures.iron_vest = LoadTexture("graphics/GAME ASSETS/ARMOR/item222.png");

	textures.leather_gauntlets = LoadTexture("graphics/GAME ASSETS/GLOVES/29.png");
	textures.iron_gauntlets = LoadTexture("graphics/GAME ASSETS/GLOVES/13.png");

	textures.leather_boots = LoadTexture("graphics/GAME ASSETS/ARMOR/item261.png");
	textures.iron_boots = LoadTexture("graphics/GAME ASSETS/ARMOR/item262.png");

	textures.apple = LoadTexture("graphics/GAME ASSETS/items/item322.png");

}

void LoadGameModels()
{
	objects.player_character = LoadModel("models/characters/moja_postac.glb");
	objects.enemy_character = LoadModel("models/postac_przeciwnika.glb");

	objects.floor_tile = LoadModel("models/terrain/floor_tile1.glb");
	objects.ceiling_tile = LoadModel("models/terrain/ceiling_tile1.glb");
	objects.wall_tile = LoadModel("models/terrain/wall_tile1.glb");

}