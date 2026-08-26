#pragma once
#include "raylib.h"



struct fx_animation
{
	Texture2D texture;

	int frame_count;
	int current_frame;
	float frame_time;
	float timer;
	bool is_playing;
	Vector2 position;

	void play(Vector2 pos)
	{
		this->position = pos;
		this->current_frame = 0;
		this->timer = 0.0;
		this->is_playing = true;
	}

	void update()
	{
		if (!is_playing)
		{
			return;
		}
		timer += GetFrameTime();
		if (timer >= frame_time)
		{
			timer = 0.0;
			current_frame++;
			if (current_frame >= frame_count)
			{
				is_playing = false;
			}
		}
	}
};

struct textureManager {

	Texture2D kopalnia;
	Texture2D player;
	Texture2D goblin;
	Texture2D skeleton;
	Texture2D troll;
	Texture2D bandits;
	Texture2D guard;
	Texture2D dragon;
	Texture2D ghoul;
	Image ghoulImage;

	Texture2D chest_t;

	Texture2D lightning_bolt;
	Texture2D fireball;
	Texture2D spikes;
	Texture2D lightning;
	Texture2D explosion;
	Texture2D ignite;
	Texture2D sun_strike;

	Texture2D lightning_bolt_icon;
	Texture2D fireball_icon;
	Texture2D spikes_icon;
	Texture2D lightning_icon;
	Texture2D explosion_icon;
	Texture2D ignite_icon;
	Texture2D sun_strike_icon;

	Texture2D long_sword;
	Texture2D short_sword;

	Texture2D leather_vest;
	Texture2D iron_vest;

	Texture2D leather_gauntlets;
	Texture2D iron_gauntlets;
	
	Texture2D leather_boots;
	Texture2D iron_boots;

	Texture2D leather_helm;
	Texture2D iron_helm;


	Texture2D wooden_shield;
	Texture2D steel_shield;

	Texture2D apple;



	Texture2D wall_tile_tex;
	Texture2D floor_tile_tex;
	Texture2D ceiling_tile_tex;

	Texture2D inventory_UI;
	Texture2D man_silhouette;

	Shader outlineShader;
	Shader fogShader;


	
};

struct ModelManager {

	Model player_character;
	Model enemy_character;
	Model floor_tile;
	Model wall_tile;
	Model ceiling_tile;
	Model m_chest;

	Model trapdoor;
	ModelAnimation* trapdoor_open_animation;

};


extern fx_animation global_fx;
extern textureManager textures;
extern ModelManager objects;

void LoadGameTextures();
void LoadGameModels();
int GetSpriteFrameIndex(Vector3 enemyPos, Vector3 enemyForward, Vector3 cameraPos, int lastFrame);

 