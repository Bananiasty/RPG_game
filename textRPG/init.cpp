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

	auto& start_node = world_map[1];

	float start_tile_x = start_node.room_x + (start_node.room_width / 2);
	float start_tile_y = start_node.room_y + (start_node.room_length / 2);

	c->position.x = (float)start_tile_x * 2.0f;
	c->position.z = (float)start_tile_y * 2.0f;
}

void exploration::loot_init()
{
	item_pool.clear();

	item_pool.push_back(new helm("Leather helm", "Normal", 10, &textures.leather_helm, 1, 15));
	item_pool.push_back(new helm("Iron helm", "Normal", 25, &textures.iron_helm, 2, 30));

	item_pool.push_back(new vest("Leather vest", "Normal", 20, &textures.leather_vest, 1, 0));
	item_pool.push_back(new vest("Iron vest", "Normal", 50, &textures.iron_vest, 2, 5));


	item_pool.push_back(new boots("Leather boots", "Normal", 7, &textures.leather_boots, 1, 0));
	item_pool.push_back(new boots("Iron boots", "Normal", 15, &textures.iron_boots, 2, 2));

	item_pool.push_back(new gauntlets("Leather gloves", "Normal", 8, &textures.leather_gauntlets, 1, 0));
	item_pool.push_back(new gauntlets("Iron gloves", "Normal", 18, &textures.iron_gauntlets, 2, 5));

	item_pool.push_back(new weapon("Short sword", "Normal", 10, &textures.short_sword, 0, 2));
	item_pool.push_back(new weapon("Long sword", "Normal", 30, &textures.long_sword, 0, 4));

	item_pool.push_back(new shield("Wooden shield", "Normal", 5, &textures.wooden_shield, 1, 5));
	item_pool.push_back(new shield("Steel shield", "Normal", 30, &textures.steel_shield, 5, 15));

	item_pool.push_back(new health_potion("HP potion", "Normal", 20, &textures.apple, 50));

	item_pool.push_back(new combat_scroll("Thunder bolt", "Rare", 120, &textures.lightning_bolt_icon, 40, &textures.lightning_bolt, 10, 0.10));
	item_pool.push_back(new combat_scroll("Sun Strike", "Rare", 120, &textures.sun_strike_icon, 40, &textures.sun_strike, 10, 0.10));
	item_pool.push_back(new combat_scroll("Explosion", "Rare", 120, &textures.explosion_icon, 40, &textures.explosion, 10, 0.10));
	item_pool.push_back(new combat_scroll("Spikes", "Rare", 120, &textures.spikes_icon, 40, &textures.spikes, 10, 0.10));
	item_pool.push_back(new combat_scroll("Fire ball", "Rare", 120, &textures.fireball_icon, 40, &textures.fireball, 8, 0.10));
	item_pool.push_back(new combat_scroll("Ignite", "Rare", 120, &textures.ignite_icon, 40, &textures.ignite, 10, 0.10));
	item_pool.push_back(new combat_scroll("Lightning bolt", "Rare", 120, &textures.lightning_icon,  40, &textures.lightning, 10, 0.10));


	bohater.bag->add_item(new shield("Wooden shield", "Normal", 5, &textures.wooden_shield, 1, 5));
	bohater.bag->add_item(new shield("Steel shield", "Normal", 30, &textures.steel_shield, 5, 15));
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
	enemy_pool.push_back(new ghoul({ .id = 1, .name = "Ghoul", .hp = 40, .armor = 0, .damage = 5, .block_chance = 10, .crit_chance = 5, .dodge_chance = 0, .reduced_head_damage = 0, .texture = textures.ghoul, .rotation = 0.0f, .level = 1, .description = "..." }));

}
void exploration::world_map_init()
{
	
	loot_init();
	enemies_init();

	add_Node({ .id = 1, .left = 2,.s_chest = rand_loot(nullptr, {21.0f, 0, 21.0f}), .dungeon_pos = {10,10}, .room_size = {5, 5} });
	add_Node({ .id = 2, .enemy_id = 1, .right = 3,  .dungeon_pos = { 10, 25 }, .room_size = { 6, 6 } });
	add_Node({ .id = 3, .left = 4, .right = 5, .dungeon_pos = { 10, 40 }, .room_size = { 8, 8 } });
	add_Node({ .id = 4, .left = 5, .dungeon_pos = { 25, 40 }, .room_size = { 6, 6 } });
	add_Node({ .id = 5, .left = 6, .right = 8, .s_chest = rand_loot(nullptr, {21.0f, 0, 111.0f}), .dungeon_pos = { 10, 55 }, .room_size = { 7, 7 } });
	add_Node({ .id = 6, .left = 7, .dungeon_pos = { 25, 55 }, .room_size = { 6, 6 } });
	add_Node({ .id = 7, .left = 8, .dungeon_pos = { 25, 70 }, .room_size = { 6, 6 } });
	add_Node({ .id = 8, .s_chest = rand_loot(nullptr, {21.0f, 0, 141.0f}), .dungeon_pos = { 10, 70 }, .room_size = { 10, 10 } });

	//draw_dungeon_map();
}




