#include "TextureManager.h"

fx_animation global_fx;
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

	textures.chest_t = LoadTexture("graphics/chest.png");
	SetTextureFilter(textures.dragon, TEXTURE_FILTER_BILINEAR);

	textures.lightning = LoadTexture("graphics/SCROLLS/1 Lightning/Lightning.png");
	textures.lightning_bolt = LoadTexture("graphics/SCROLLS/2 Lightning bolt/Lightning-bolt.png");
	textures.sun_strike = LoadTexture("graphics/SCROLLS/4 Sun strike/Sun-strike.png");
	textures.explosion = LoadTexture("graphics/SCROLLS/5 Explosion/Explosion.png");
	textures.spikes = LoadTexture("graphics/SCROLLS/6 Spikes/Spikes.png");
	textures.fireball = LoadTexture("graphics/SCROLLS/10 Fire ball/Fire-ball.png");
	textures.ignite = LoadTexture("graphics/SCROLLS/7 Fire wall/Fire-wall.png");

}