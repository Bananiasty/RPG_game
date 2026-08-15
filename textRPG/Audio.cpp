#include "raylib.h"
#include "AudioManager.h"
#include <unordered_map>
#include <string>



AudioManager audio;

void AudioManager::init() 
{
	InitAudioDevice(); 
}

void LoadGameAudio()
{
	audio.init();
	audio.load_music(MusicID::EXPLORATION, "music/background/dungeon.wav");

	audio.load_sound(SoundID::ENEMY_DETECT, "music/effects/chase/chase_1.wav");

	audio.load_sound(SoundID::PLAYER_HIT, "music/effects/attack/weapon_attack_1.wav");
	/*audio.load_sound(SoundID::PLAYER_HIT, "music/effects/attack/weapon_attack_2.wav");
	audio.load_sound(SoundID::PLAYER_HIT, "music/effects/attack/weapon_attack_3.wav");
	audio.load_sound(SoundID::PLAYER_HIT, "music/effects/attack/weapon_attack_4.wav");
	audio.load_sound(SoundID::PLAYER_HIT, "music/effects/attack/weapon_attack_5.wav");*/

	//audio.load_sound(SoundID::ENEMY_HIT, "music/effects/attack/weapon_attack_1.wav");
	audio.load_sound(SoundID::ENEMY_HIT, "music/effects/attack/weapon_attack_2.wav");
	/*audio.load_sound(SoundID::ENEMY_HIT, "music/effects/attack/weapon_attack_3.wav");
	audio.load_sound(SoundID::ENEMY_HIT, "music/effects/attack/weapon_attack_4.wav");
	audio.load_sound(SoundID::ENEMY_HIT, "music/effects/attack/weapon_attack_5.wav");*/

}

AudioManager::~AudioManager() {
	for (auto& [id, music] : music_tracks) {
		UnloadMusicStream(music);
	}
	for (auto& [id, sound] : sound_effects) {
		UnloadSound(sound);
	}
	CloseAudioDevice();
}

void AudioManager::load_music(MusicID id, const char* filepath) {
	Music m = LoadMusicStream(filepath);
	SetMusicVolume(m, music_volume);
	music_tracks[id] = m;
}

void AudioManager::load_sound(SoundID id, const char* filepath) {
	Sound s = LoadSound(filepath);
	SetSoundVolume(s, sound_volume);
	sound_effects[id] = s;
}

void AudioManager::play_sound(SoundID id) {
	auto it = sound_effects.find(id);
	if (it != sound_effects.end()) {
		PlaySound(it->second);
	}
}

void AudioManager::play_music(MusicID id) {
	auto it = music_tracks.find(id);
	if (it == music_tracks.end())
	{
		return;
	}

	if (is_music_playing && current_music_id != id) {
		StopMusicStream(music_tracks[current_music_id]);
	}
	current_music_id = id;
	PlayMusicStream(it->second);
	is_music_playing = true;
}


void AudioManager::update()
{
	if (is_music_playing) {
		auto it = music_tracks.find(current_music_id);
		if (it != music_tracks.end()) {
			UpdateMusicStream(it->second);
		}
	}
}

void AudioManager::set_music_volume(float vol)
{
	music_volume = vol;
	for (auto& [id, music] : music_tracks) {
		SetMusicVolume(music, music_volume);
	}
}
void AudioManager::set_sound_volume(float vol)
{
	sound_volume = vol;
	for (auto& [id, sound] : sound_effects) {
		SetSoundVolume(sound, sound_volume);
	}
}

bool AudioManager::is_sound_playing(SoundID id)
{
	auto it = sound_effects.find(id);
	if (it != sound_effects.end())
	{
		return IsSoundPlaying(it->second);
	}
	return false;
}