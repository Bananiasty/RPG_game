#include "TextureManager.h"

textureManager textures;

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
}