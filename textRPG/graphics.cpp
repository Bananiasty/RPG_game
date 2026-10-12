#include "graphics.h"
#include "textureManager.h"
#include "AudioManager.h"
#include "ui/BattleUI.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void graphics_init()
{
	const int screenWidth = 1280;
	const int screenHeight = 720;
	InitWindow(screenWidth, screenHeight, "textRPG");
	SetExitKey(KEY_NULL);
	DisableCursor();
	SetTargetFPS(60);

	LoadGameTextures();
	LoadGameModels();
	LoadGameAudio();
	body_UI.init();
}
