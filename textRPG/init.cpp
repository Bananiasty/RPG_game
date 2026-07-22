#include <iostream>
#include <map>
#include <string>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "graphics.h"
#include "raylib.h"
#include "TextureManager.h"


void exploration::player_stats_init(player* c)
{
	c->set_max_health(c->get_health());
	c->set_max_mana(c->get_mana());
}

void exploration::loot_init()
{
	item_pool.clear();

	item_pool.push_back(new helm("Leather helm", "Normal", 10, &textures.leather_helm, 1, 0));
	item_pool.push_back(new helm("Iron helm", "Normal", 25, &textures.iron_helm, 2, 5));

	item_pool.push_back(new vest("Leather vest", "Normal", 20, &textures.leather_vest, 1, 0));
	item_pool.push_back(new vest("Iron vest", "Normal", 50, &textures.iron_vest, 2, 5));


	item_pool.push_back(new boots("Leather boots", "Normal", 7, &textures.leather_boots, 1, 0));
	item_pool.push_back(new boots("Iron boots", "Normal", 15, &textures.leather_boots, 2, 2));

	item_pool.push_back(new gauntlets("Leather gloves", "Normal", 8, &textures.leather_gauntlets, 1, 0));
	item_pool.push_back(new gauntlets("Iron gloves", "Normal", 18, &textures.iron_gauntlets, 2, 5));

	item_pool.push_back(new weapon("Short sword", "Normal", 10, &textures.short_sword, 0, 2));
	item_pool.push_back(new weapon("Long sword", "Normal", 30, &textures.long_sword, 0, 4));

	item_pool.push_back(new health_potion("HP potion", "Normal", 20, &textures.apple, 50));

	item_pool.push_back(new combat_scroll("Thunder bolt", "Rare", 120, &textures.lightning_bolt_icon, 40, &textures.lightning_bolt, 10, 0.10));
	item_pool.push_back(new combat_scroll("Sun Strike", "Rare", 120, &textures.sun_strike_icon, 40, &textures.sun_strike, 10, 0.10));
	item_pool.push_back(new combat_scroll("Explosion", "Rare", 120, &textures.explosion_icon, 40, &textures.explosion, 10, 0.10));
	item_pool.push_back(new combat_scroll("Spikes", "Rare", 120, &textures.spikes_icon, 40, &textures.spikes, 10, 0.10));
	item_pool.push_back(new combat_scroll("Fire ball", "Rare", 120, &textures.fireball_icon, 40, &textures.fireball, 8, 0.10));
	item_pool.push_back(new combat_scroll("Ignite", "Rare", 120, &textures.ignite_icon, 40, &textures.ignite, 10, 0.10));
	item_pool.push_back(new combat_scroll("Lightning bolt", "Rare", 120, &textures.lightning_icon,  40, &textures.lightning, 10, 0.10));

	bohater.bag->add_item(new weapon("Short sword", "Normal", 10, &textures.short_sword, 0, 2)); 
	bohater.bag->add_item(new gauntlets("Iron gloves", "Normal", 18, &textures.iron_gauntlets, 2, 5));
	bohater.bag->add_item(new vest("Iron vest", "Normal", 50, &textures.iron_vest, 2, 5));
	bohater.bag->add_item(new helm("Iron helm", "Normal", 25, &textures.iron_helm, 2, 5));
	bohater.bag->add_item(new vest("Leather vest", "Normal", 20, &textures.leather_vest, 1, 0));
	bohater.bag->add_item(new boots("Leather boots", "Normal", 7, &textures.leather_boots, 1, 0));
	bohater.bag->add_item(new health_potion("Apple", "Normal", 20, &textures.apple, 50));
	bohater.bag->add_item(new weapon("Long sword", "Normal", 30, &textures.long_sword, 0, 4));
	bohater.bag->add_item(new combat_scroll("Lightning bolt", "Rare", 120, &textures.lightning_icon, 40, &textures.lightning, 10, 0.10));

	
	
	bohater.sort_bag();

}
void exploration::enemies_init()
{

	enemy_pool.push_back(new enemy(1, "Szkielet", 15, 0, 2, 5, 0, 0, 0, textures.skeleton, { 45.0f, 1.0f, 73.0f }, 0, 1, "Przed toba stoi otepialy szkielet, chyba czegos broni."));
	enemy_pool.push_back(new enemy(2, "Goblin", 30, 0, 4, 7, 5, 10, 15, textures.goblin, { 26.0f, 1.0f, 56.0f }, 0, 1, "Z krzaka wyskoczyl wsciekly Goblin!!!"));
	enemy_pool.push_back(new enemy(3, "Troll", 80, 0, 0, 12, 15, 25, 0, textures.troll, { 28.0f, 1.0f, 88.0f }, 0, 2, "Troll... wyglada na twardego."));
	enemy_pool.push_back(new enemy(4, "Bandyci", 50, 0, 2, 10, 0, 25, 30, textures.bandits, { 56.0f, 1.0f, 86.0f }, 0, 2, "Bandyci chca cie ograbic!!!"));
	enemy_pool.push_back(new enemy(5, "Straznik", 50, 0, 6, 10, 40, 0, 0, textures.guard, { 56.0f, 1.0f, 116.0f }, 0, 4, "Straznik nie chce cie przepuscic przez most."));
	enemy_pool.push_back(new enemy(6, "Smok", 80, 0, 10, 15, 0, 0, 0, textures.dragon, { 30.0f, 1.0f, 150.0f }, 0, 10, "Final boss!!!"));
}
void exploration::world_map_init()
{
	player_stats_init(&bohater);
	loot_init();
	enemies_init();

	enemy* szkielet = enemy_pool[0]->clone();
	enemy* goblin = enemy_pool[1]->clone();
	enemy* troll = enemy_pool[2]->clone();
	enemy* bandyci = enemy_pool[3]->clone();
	enemy* straznik = enemy_pool[4]->clone();
	enemy* smok = enemy_pool[5]->clone();


	add_Node(1, 2, -1, -1, true, 550, 530, nullptr, 20, 34, 5, 5);

	add_Node(2, -1, 3, -1, true, 600, 500, nullptr, 10, 25, 6, 6);

	add_Node(3, 4, 5, -1, false, 600, 400, nullptr, 10, 40, 8, 8);

	add_Node(4, 5, -1, -1, false, 450, 300, nullptr, 25, 40, 6, 6);

	add_Node(5, 6, 8, -1, false, 750, 300, nullptr, 10, 55, 7, 7);

	add_Node(6, 7, -1, -1, false, 750, 200, nullptr, 25, 55, 6, 6);

	add_Node(7, 8, -1, -1, false, 750, 100, nullptr, 25, 70, 6, 6);

	add_Node(8, -1, -1, -1, false, 600, 100, nullptr, 10, 70, 10, 10);

	map_graph();
}




