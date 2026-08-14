#pragma once
#include "raylib.h"
#include <unordered_map>
#include <string>

enum class MusicID {

	EXPLORATION,
	BATTLE,
	MENU
};

enum class SoundID {

	ENEMY_DETECT,
	PLAYER_HIT,
	ENEMY_HIT,
	CLICK
};

struct AudioManager {

	std::unordered_map<MusicID, Music> music_tracks;
	std::unordered_map<SoundID, Sound> sound_effects;

	MusicID current_music_id = MusicID::EXPLORATION;
	bool is_music_playing = false;

	float master_volume = 1.0f;
	float music_volume = 0.6f;
	float sound_volume = 1.0f;

	AudioManager() = default;
	~AudioManager();

	void init();

	void load_music(MusicID id, const char* filepath);
	void load_sound(SoundID id, const char* filepath);
	void play_sound(SoundID id);
	void play_music(MusicID id);

	void update();

	void set_music_volume(float vol);
	void set_sound_volume(float vol);

};

extern AudioManager audio;
void LoadGameAudio();