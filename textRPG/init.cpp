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

	auto& start_node = world_map[1];

	float start_tile_x = start_node.room_x + (start_node.room_width / 2);
	float start_tile_y = start_node.room_y + (start_node.room_length / 2);

	c->position.x = (float)start_tile_x * 2.0f;
	c->position.z = (float)start_tile_y * 2.0f;
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

	enemy_pool.push_back(new enemy({ .id = 1, .name = "Szkielet", .hp = 15, .mp = 0, .armor = 2, .damage = 5, .block_chance = 0, .crit_chance = 0, .dodge_chance = 0, .texture = textures.skeleton, .rotation = 0.0f, .level = 1, .description = "Przed toba stoi otepialy szkielet, chyba czegos broni." }));
	enemy_pool.push_back(new enemy({ .id = 2, .name = "Goblin", .hp = 30, .mp = 0, .armor = 4, .damage = 7, .block_chance = 5, .crit_chance = 10, .dodge_chance = 15, .texture = textures.goblin, .rotation = 0.0f, .level = 1, .description = "Z krzaka wyskoczyl wsciekly Goblin!!!" }));
	enemy_pool.push_back(new enemy({ .id = 3, .name = "Troll", .hp = 80, .mp = 0, .armor = 0, .damage = 12, .block_chance = 15, .crit_chance = 25, .dodge_chance = 0, .texture = textures.troll, .rotation = 0.0f, .level = 2, .description = "Troll... wyglada na twardego." }));
	enemy_pool.push_back(new enemy({ .id = 4, .name = "Bandyci", .hp = 50, .mp = 0, .armor = 2, .damage = 10, .block_chance = 0, .crit_chance = 25, .dodge_chance = 30, .texture = textures.bandits, .rotation = 0.0f, .level = 2, .description = "Bandyci chca cie ograbic!!!" }));
	enemy_pool.push_back(new enemy({ .id = 5, .name = "Straznik", .hp = 50, .mp = 0, .armor = 6, .damage = 10, .block_chance = 40, .crit_chance = 0, .dodge_chance = 0, .texture = textures.guard, .rotation = 0.0f, .level = 4, .description = "Straznik nie chce cie przepuscic przez most." }));
	enemy_pool.push_back(new enemy({ .id = 6, .name = "Smok", .hp = 80, .mp = 0, .armor = 10, .damage = 15, .block_chance = 0, .crit_chance = 0, .dodge_chance = 0, .texture = textures.dragon, .rotation = 0.0f, .level = 10, .description = "Final boss!!!" }));
}
void exploration::world_map_init()
{
	
	loot_init();
	enemies_init();

	add_Node({ .id = 1, .left = 2, .dungeon_pos = {10,10}, .room_size = {5, 5} });
	add_Node({ .id = 2, .enemy_id = 2, .right = 3,  .dungeon_pos = { 10, 25 }, .room_size = { 6, 6 } });
	add_Node({ .id = 3, .enemy_id = 3, .left = 4, .right = 5, .dungeon_pos = { 10, 40 }, .room_size = { 8, 8 } });
	add_Node({ .id = 4, .enemy_id = 4, .left = 5, .dungeon_pos = { 25, 40 }, .room_size = { 6, 6 } });
	add_Node({ .id = 5, .left = 6, .right = 8, .s_chest = rand_loot(nullptr, {21.0f, 1.0f, 111.0f}), .dungeon_pos = { 10, 55 }, .room_size = { 7, 7 } });
	add_Node({ .id = 6, .enemy_id = 5, .left = 7, .dungeon_pos = { 25, 55 }, .room_size = { 6, 6 } });
	add_Node({ .id = 7, .enemy_id = 6, .left = 8, .dungeon_pos = { 25, 70 }, .room_size = { 6, 6 } });
	add_Node({ .id = 8, .s_chest = rand_loot(nullptr, {21.0f, 1.0f, 141.0f}), .dungeon_pos = { 10, 70 }, .room_size = { 10, 10 } });

	//draw_dungeon_map();
}




