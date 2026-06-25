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

	item_pool.push_back(new helm("Skorzany helm", "Zwykły", 10, 1, 0));
	item_pool.push_back(new helm("Stalowy helm", "Zwykły", 25, 2, 5));
	item_pool.push_back(new helm("Helm bloku", "Rzadki", 60, 3, 15));
	item_pool.push_back(new helm("Luskowy helm", "Zwykły", 70, 4, 10));

	item_pool.push_back(new vest("Skorzana zbroja", "Zwykły", 20, 1, 0));
	item_pool.push_back(new vest("Stalowa zbroja", "Zwykły", 50, 2, 5));
	item_pool.push_back(new vest("Zbroja zycia", "Rzadki", 100, 3, 10));
	item_pool.push_back(new vest("Luskowa zbroja", "Zwykły", 110, 4, 8));

	item_pool.push_back(new boots("Skorzane buty", "Zwykły", 7, 1, 0));
	item_pool.push_back(new boots("Stalowe buty", "Zwykły", 15, 2, 2));
	item_pool.push_back(new boots("Buty szybkosci", "Rzadki", 140, 3, 25));
	item_pool.push_back(new boots("Luskowe buty", "Zwykły", 50, 4, 7));

	item_pool.push_back(new gauntlets("Skorzane rekawice", "Zwykły", 8, 1, 0));
	item_pool.push_back(new gauntlets("Stalowe rekawice", "Zwykły", 18, 2, 5));
	item_pool.push_back(new gauntlets("Rekawice zabojcy", "Rzadki", 120, 3, 50));
	item_pool.push_back(new gauntlets("Luskowe rekawice", "Zwykły", 40, 4, 12));

	item_pool.push_back(new weapon("Krotki miecz", "Zwykły", 10, 0, 2));
	item_pool.push_back(new weapon("Dlugi miecz", "Zwykły", 30, 0, 4));
	item_pool.push_back(new weapon("Miecz bohatera", "Rzadki", 160, 0, 6));
	item_pool.push_back(new weapon("Topor", "Zwykły", 40, 0, 4));

	item_pool.push_back(new health_potion("Mikstura Zycia", "Zwykły", 20, 50));

	bohater.bag->add_item(new weapon("Miecz bohatera", "Zwykły", 160, 0, 6));
	bohater.bag->add_item(new vest("Luskowa zbroja", "Zwykły", 110, 4, 8));
	bohater.bag->add_item(new health_potion("Mikstura Zycia", "Zwykły", 20, 50));
	bohater.bag->add_item(new health_potion("Mikstura Zycia", "Zwykły", 20, 50));

	bohater.sort_bag();

}
void exploration::enemies_init()
{

	enemy_pool.push_back(new enemy("Szkielet", 15, 0, 2, 5, 0, 0, 0, textures.skeleton, 1, "Przed toba stoi otepialy szkielet, chyba czegos broni."));
	enemy_pool.push_back(new enemy("Goblin", 30, 0, 4, 7, 5, 10, 15, textures.goblin, 1, "Z krzaka wyskoczyl wsciekly Goblin!!!"));
	enemy_pool.push_back(new enemy("Troll", 80, 0, 0, 12, 15, 25, 0, textures.troll, 2, "Troll... wyglada na twardego."));
	enemy_pool.push_back(new enemy("Bandyci", 50, 0, 2, 10, 0, 25, 30, textures.bandits, 2, "Bandyci chca cie ograbic!!!"));
	enemy_pool.push_back(new enemy("Straznik", 50, 0, 6, 10, 40, 0, 0, textures.guard, 4, "Straznik nie chce cie przepuscic przez most."));
	enemy_pool.push_back(new enemy("Smok", 80, 0, 10, 15, 0, 0, 0, textures.dragon, 10, "Final boss!!!"));
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


	add_Node(1, "Baza wypadowa", 2, -1, 1, true, 550, 530, nullptr, nullptr);
	add_Node(2, "Las", 3, 4, 1, true, 550, 530, nullptr, nullptr);
	add_Node(3, "Kopalnia", -1, -1, 2, false, 550, 530, szkielet, rand_loot(szkielet));
	add_Node(4, "Pole", 6, 5, 2, false, 550, 530, goblin, nullptr);
	add_Node(5, "Legowisko Trola", 7, -1, 4, false, 550, 530, troll, rand_loot(troll));
	add_Node(6, "Oboz Bandytow", -1, 7, 4, false, 550, 530, bandyci, rand_loot(bandyci));
	add_Node(7, "Most", 8, -1, 5, false, 550, 530, straznik, rand_loot(straznik));
	add_Node(8, "Smocza nora", -1, -1, 7, false, 550, 530, smok, rand_loot(smok));

	map_graph();
}



