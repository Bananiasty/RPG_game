#include "TextureManager.h"

textureManager textures;

void LoadGameTextures()
{
	textures.kopalnia = LoadTexture("graphics/kopalnia.png");

	textures.player = LoadTexture("graphics/bohater.png");
	textures.goblin = LoadTexture("graphics/goblin.png");
	textures.skeleton = LoadTexture("graphics/szkielet.png");
	textures.troll = LoadTexture("graphics/troll.png");
	textures.bandits = LoadTexture("graphics/bandyci.png");
	textures.guard = LoadTexture("graphics/straznik.png");
	textures.dragon = LoadTexture("graphics/smok.png");
}