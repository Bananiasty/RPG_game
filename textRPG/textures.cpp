#include "TextureManager.h"
#include <iostream>
#include "character.h"

fx_animation global_fx;
textureManager textures;
ModelManager objects;

void LoadGameTextures()
{


	textures.inventory_UI = LoadTexture("graphics/UI/inventory_UI.png");

	textures.player = LoadTexture("graphics/textures/enemies/bohater.png");
	SetTextureFilter(textures.player, TEXTURE_FILTER_BILINEAR);

	textures.goblin = LoadTexture("graphics/textures/enemies/goblin.png");
	SetTextureFilter(textures.goblin, TEXTURE_FILTER_BILINEAR);

	textures.skeleton = LoadTexture("graphics/textures/enemies/szkielet.png");
	SetTextureFilter(textures.skeleton, TEXTURE_FILTER_BILINEAR);

	textures.troll = LoadTexture("graphics/textures/enemies/troll.png");
	SetTextureFilter(textures.troll, TEXTURE_FILTER_BILINEAR);

	textures.bandits = LoadTexture("graphics/textures/enemies/bandyci.png");
	SetTextureFilter(textures.bandits, TEXTURE_FILTER_BILINEAR);

	textures.guard = LoadTexture("graphics/textures/enemies/straznik.png");
	SetTextureFilter(textures.guard, TEXTURE_FILTER_BILINEAR);

	textures.dragon = LoadTexture("graphics/textures/enemies/smok.png");
	SetTextureFilter(textures.dragon, TEXTURE_FILTER_BILINEAR);

	textures.chest_t = LoadTexture("graphics/textures/enemies/chest.png");
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

	textures.wooden_shield = LoadTexture("graphics/GAME ASSETS/ARMOR/item197.png");
	textures.steel_shield = LoadTexture("graphics/GAME ASSETS/ARMOR/item199.png");

	textures.apple = LoadTexture("graphics/GAME ASSETS/items/item322.png");

	textures.wall_tile_tex = LoadTexture("graphics/textures/terrain/dark_rock_128.png");
	textures.floor_tile_tex = LoadTexture("graphics/textures/terrain/cobblestone_floor_128.png");
	textures.ceiling_tile_tex = LoadTexture("graphics/textures/terrain/dark_rock_128.png");

	textures.man_silhouette = LoadTexture("graphics/UI/man_silhouette.png");

	textures.ghoul = LoadTexture("graphics/textures/enemies/ghoul_64/ghoul_limbs_3.png");
	textures.ghoulImage = LoadImage("graphics/textures/enemies/ghoul_64/ghoul_limbs_3.png");

			
	// Kontury przeciwnikow
	textures.outlineShader = LoadShader("graphics/shaders/outline.vs", "graphics/shaders/outline.fs");

	int textureSizeLoc = GetShaderLocation(textures.outlineShader, "textureSize");
	int outlineColorLoc = GetShaderLocation(textures.outlineShader, "outlineColor");
	int outlineSizeLoc = GetShaderLocation(textures.outlineShader, "outlineSize");

	float frameWidth = (float)textures.ghoul.width / 8.0f;
	float frameHeight = (float)textures.ghoul.height;

	float texSize[2] = { frameWidth, frameHeight };
	SetShaderValue(textures.outlineShader, textureSizeLoc, texSize, SHADER_UNIFORM_VEC2);

	float Outline[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	SetShaderValue(textures.outlineShader, outlineColorLoc, Outline, SHADER_UNIFORM_VEC4);

	float outlineSizeValue = 1.0f;
	SetShaderValue(textures.outlineShader, outlineSizeLoc, &outlineSizeValue, SHADER_UNIFORM_FLOAT);



	//Widocznosc
	textures.fogShader = LoadShader("graphics/shaders/lighting.vs","graphics/shaders/lighting.fs");


	textures.fogShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(textures.fogShader, "matModel");
	textures.fogShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(textures.fogShader, "mvp");

	int viewPosLoc = GetShaderLocation(textures.fogShader, "viewPos");
	int minDistLoc = GetShaderLocation(textures.fogShader, "minDistance");
	int maxDistLoc = GetShaderLocation(textures.fogShader, "maxDistance");

	float minDist = 2.0f;
	float maxDist = 15.0f;

	SetShaderValue(textures.fogShader, minDistLoc, &minDist, SHADER_UNIFORM_FLOAT);
	SetShaderValue(textures.fogShader, maxDistLoc, &maxDist, SHADER_UNIFORM_FLOAT);

}

void LoadGameModels()
{
	objects.player_character = LoadModel("graphics/models/characters/moja_postac.glb");
	objects.enemy_character = LoadModel("graphics/models/postac_przeciwnika.glb");

	objects.m_chest = LoadModel("graphics/models/objects/treasure_chest_128.glb");
	objects.m_chest.materials[1].shader = textures.fogShader;

	objects.floor_tile = LoadModel("graphics/models/terrain/floor_tile1.glb");

	objects.floor_tile.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = textures.floor_tile_tex;
	objects.floor_tile.materials[1].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
	objects.floor_tile.materials[1].shader = textures.fogShader;

	objects.ceiling_tile = LoadModel("graphics/models/terrain/ceiling_tile1.glb");
	objects.ceiling_tile.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = textures.ceiling_tile_tex;
	objects.ceiling_tile.materials[1].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
	objects.ceiling_tile.materials[1].shader = textures.fogShader;


	objects.wall_tile = LoadModel("graphics/models/terrain/wall_tile.glb");

	objects.wall_tile.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = textures.wall_tile_tex;
	objects.wall_tile.materials[1].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
	objects.wall_tile.materials[1].shader = textures.fogShader;

	
}

int GetSpriteFrameIndex(Vector3 enemyPos, Vector3 enemyForward, Vector3 cameraPos, int lastFrame)
{
	float dx = cameraPos.x - enemyPos.x;
	float dz = cameraPos.z - enemyPos.z;

	float angleToCamera = atan2f(dz, dx) * RAD2DEG;
	float enemyAngle = atan2f(enemyForward.z, enemyForward.x) * RAD2DEG;

	float relativeAngle = angleToCamera - enemyAngle;

	while (relativeAngle < 0.0f) relativeAngle += 360.0f;
	while (relativeAngle >= 360.0f) relativeAngle -= 360.0f;

	int newFrame = (int)((relativeAngle + 22.5f) / 45.0f) % 8;

	if (newFrame == lastFrame)
	{
		return newFrame;
	}


	float targetCenterAngle = newFrame * 45.0f;
	float angleDiff = std::abs(relativeAngle - targetCenterAngle);

	// Normalizacja ró¿nicy k¹tów
	if (angleDiff > 180.0f) angleDiff = 360.0f - angleDiff;

	float margin = 6.0f;

	if (angleDiff > (22.5f - margin))
	{
		return lastFrame;
	}

	return newFrame;
}