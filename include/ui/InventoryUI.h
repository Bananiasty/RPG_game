#pragma once
#include "entities/Player.h"

class inventory_state;


void draw_inventory_ui(player& p, inventory_state* inv);


struct equipment_slots
{
	Rectangle helm_slot;
	Rectangle weapon_slot;
	Rectangle shield_slot;
	Rectangle vest_slot;
	Rectangle gloves_slot;
	Rectangle boots_slot;
	Rectangle accessory_1;
	Rectangle accessory_2;
	Rectangle accessory_3;
};

equipment_slots equipment_slots_placement();

void handle_single_slot(item* item_ptr, Rectangle slot, Vector2 mouse_pos, player& p, item*& hovered_item);
void equipment_slots_handle(player& p, const equipment_slots& slots, Vector2 mouse_pos, item*& hovered_item);
void generic_tab_handler(std::vector<item*>& item_list, player& p, inventory_state* inv, Vector2 mouse_pos, Vector2 window_center, item*& hovered_item);

enum class InventoryTab
{
	NONE,
	EQUIPMENT,
	ITEMS,
	FOOD,
	BOOKS
};


