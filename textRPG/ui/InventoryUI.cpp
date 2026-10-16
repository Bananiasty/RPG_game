#include "ui/InventoryUI.h"
#include "ui/ItemTooltipUI.h"
#include "ui/HUD.h"
#include "ui/BattleUI.h"
#include "textureManager.h"
#include "graphics.h"
#include "gamestates.h"
#include "inventory/Equipment.h"
#include "inventory/Consumable.h"
#include "inventory/Scroll.h"


void draw_inventory_ui(player& p, inventory_state* inv)
{
	equipment_slots slots_coordinates = equipment_slots_placement();
	p.sort_bag();

	Texture2D inventory = textures.inventory_UI;
	Texture2D item_icon;

	Vector2 mouse_pos = GetVirtualMousePosition();
	Vector2 window_center = { (float)(GAME_WIDTH * 0.5f - inventory.width * 0.5f), (float)(GAME_HEIGHT * 0.5f - inventory.height * 0.5f) };

	item* hovered_item = nullptr;

	Rectangle equipment_button = { window_center.x + inventory.width * 0.057f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle item_button = { window_center.x + inventory.width * 0.181f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle food_button = { window_center.x + inventory.width * 0.308f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle books_button = { window_center.x + inventory.width * 0.437f, window_center.y + inventory.height * 0.081f, 120, 120 };

	DrawTextureEx(inventory, window_center, 0.0f, 1.0f, WHITE);
	draw_player_stats(p);

	equipment_slots_handle(p, slots_coordinates, mouse_pos, hovered_item);

	Vector2 body_pos = { GAME_WIDTH * 0.78f, GAME_HEIGHT * 0.15f };
	body_UI.draw(p, body_pos, 2.0f);

	if (!inv->equipment_tab && !inv->items_tab && !inv->food_tab && !inv->books_tab)
	{
		inv->equipment_tab = true;
	}

	InventoryTab hovered_tab = InventoryTab::NONE;
	Rectangle hovered_rect = {};

	if (CheckCollisionPointRec(mouse_pos, equipment_button))
	{
		hovered_tab = InventoryTab::EQUIPMENT;
		hovered_rect = equipment_button;
	}
	else if (CheckCollisionPointRec(mouse_pos, item_button))
	{
		hovered_tab = InventoryTab::ITEMS;
		hovered_rect = item_button;
	}
	else if (CheckCollisionPointRec(mouse_pos, food_button))
	{
		hovered_tab = InventoryTab::FOOD;
		hovered_rect = food_button;
	}
	else if (CheckCollisionPointRec(mouse_pos, books_button))
	{
		hovered_tab = InventoryTab::BOOKS;
		hovered_rect = books_button;
	}

	if (inv->equipment_tab) DrawRectangleLinesEx(equipment_button, 3.0f, GRAY);
	else if (inv->items_tab) DrawRectangleLinesEx(item_button, 3.0f, GRAY);
	else if (inv->food_tab) DrawRectangleLinesEx(food_button, 3.0f, GRAY);
	else if (inv->books_tab) DrawRectangleLinesEx(books_button, 3.0f, GRAY);

	if (hovered_tab != InventoryTab::NONE)
	{
		DrawRectangleLinesEx(hovered_rect, 5.0f, WHITE);

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			switch (hovered_tab)
			{
			case InventoryTab::EQUIPMENT:
				inv->equipment_tab = true;
				inv->items_tab = false;
				inv->food_tab = false;
				inv->books_tab = false;
				break;
			case InventoryTab::ITEMS:
				inv->equipment_tab = false;
				inv->items_tab = true;
				inv->food_tab = false;
				inv->books_tab = false;
				break;
			case InventoryTab::FOOD:
				inv->equipment_tab = false;
				inv->items_tab = false;
				inv->food_tab = true;
				inv->books_tab = false;
				break;
			case InventoryTab::BOOKS:
				inv->equipment_tab = false;
				inv->items_tab = false;
				inv->food_tab = false;
				inv->books_tab = true;
				break;
			default:
				break;
			}
		}
	}
	

	auto& equipment = p.equipment->items;

	int startX = 80;
	int startY = 250;
	int btnWidth = 60;
	int btnHeight = 60;
	int bottom_padding = 15;
	int right_padding = 140;

	if (inv->equipment_tab == true)
	{
		int renderIndex = 0;
		for (int i = 0; i < equipment.size(); i++)
		{
			if (equipment[i]->is_equipped()) continue;

			item_icon = *(equipment[i]->icon_texture);

			int col = renderIndex / 8;
			int row = renderIndex % 8;
			Rectangle itemRect = {
				window_center.x + (float)(startX + col * (btnWidth + right_padding)),
				window_center.y + (float)(startY + row * (btnHeight + bottom_padding)),
				(float)btnWidth,
				(float)btnHeight
			};
			bool is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

			if (is_item_hovered)
			{
				if (dynamic_cast<helm*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(slots_coordinates.helm_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(slots_coordinates.helm_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<weapon*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(slots_coordinates.weapon_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(slots_coordinates.weapon_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<shield*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(slots_coordinates.shield_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(slots_coordinates.shield_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<vest*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(slots_coordinates.vest_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(slots_coordinates.vest_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<gauntlets*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(slots_coordinates.gloves_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(slots_coordinates.gloves_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<boots*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(slots_coordinates.boots_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(slots_coordinates.boots_slot, 2.5f, WHITE);
				}

				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					equipment[i]->use(&p);
					p.sort_bag();
					hovered_item = nullptr;
					break;
				}
				if (IsKeyPressed(KEY_D))
				{
					inv->pending_drops.push_back(equipment[i]);
					p.drop_item(equipment[i]);
					p.sort_bag();
					hovered_item = nullptr;
					break;
				}
			}

			if (!equipment[i]->is_equipped())
			{
				if (item_icon.id > 0)
				{
					float scale = (float)(btnHeight - 10) / item_icon.height;
					DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
				}

				if (is_item_hovered)
				{
					hovered_item = equipment[i];
					DrawRectangleLinesEx(itemRect, 2.0f, RAYWHITE);
				}
			}
			renderIndex++;
		}
	}
	else
	{
		std::vector<item*>* active_list = nullptr;

		if (inv->items_tab)      active_list = &p.inv_items->items;
		else if (inv->food_tab)  active_list = &p.food->items;
		else if (inv->books_tab) active_list = &p.books->items;

		if (active_list != nullptr)
		{
			generic_tab_handler(*active_list, p, inv, mouse_pos, window_center, hovered_item);
		}
	}

	if (hovered_item != nullptr)
	{
		draw_item_tooltip(hovered_item, mouse_pos);
	}
}




void generic_tab_handler(std::vector<item*>& item_list, player& p, inventory_state* inv, Vector2 mouse_pos, Vector2 window_center, item*& hovered_item)
{
	const int startX = 80;
	const int startY = 250;
	const int btnWidth = 60;
	const int btnHeight = 60;
	const int bottom_padding = 15;
	const int right_padding = 140;

	for (size_t i = 0; i < item_list.size(); i++)
	{
		item* current_item = item_list[i];
		Texture2D item_icon = *(current_item->icon_texture);

		int row = (int)i / 5;
		int col = (int)i % 5;
		Rectangle itemRect = {
			window_center.x + (float)(startX + col * (btnWidth + right_padding)),
			window_center.y + (float)(startY + row * (btnHeight + bottom_padding)),
			(float)btnWidth,
			(float)btnHeight
		};

		bool is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

		if (is_item_hovered)
		{
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				current_item->use(&p);
				p.sort_bag();
				hovered_item = nullptr;
				break;
			}
			if (IsKeyPressed(KEY_D))
			{
				inv->pending_drops.push_back(current_item);
				p.drop_item(current_item);
				p.sort_bag();
				hovered_item = nullptr;
				break;
			}
		}

		if (item_icon.id > 0)
		{
			float scale = (float)(btnHeight - 10) / item_icon.height;
			DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
		}

		if (is_item_hovered)
		{
			hovered_item = current_item;
			DrawRectangleLinesEx(itemRect, 2.0f, RAYWHITE);
		}
	}
}


equipment_slots equipment_slots_placement()
{
	Texture2D inventory = textures.inventory_UI;

	Vector2 window_center = { (float)(GAME_WIDTH * 0.5f - inventory.width * 0.5f), (float)(GAME_HEIGHT * 0.5f - inventory.height * 0.5f) };

	const float SLOT_SIZE = 75.0f;

	const float HELM_OFFSET_X = 1039.5f;
	const float HELM_OFFSET_Y = 75.0f;
	Rectangle helm_slot = { window_center.x + HELM_OFFSET_X, window_center.y + HELM_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float WEAPON_OFFSET_X = 919.5f;
	const float WEAPON_OFFSET_Y = 186.0f;
	Rectangle weapon_slot = { window_center.x + WEAPON_OFFSET_X, window_center.y + WEAPON_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float SHIELD_OFFSET_X = 1159.5f;
	const float SHIELD_OFFSET_Y = 186.0f;
	Rectangle shield_slot = { window_center.x + SHIELD_OFFSET_X, window_center.y + SHIELD_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float VEST_OFFSET_X = 1039.5f;
	const float VEST_OFFSET_Y = 229.5f;
	Rectangle vest_slot = { window_center.x + VEST_OFFSET_X, window_center.y + VEST_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float GLOVES_OFFSET_X = 918.0f;
	const float GLOVES_OFFSET_Y = 342.0f;
	Rectangle gloves_slot = { window_center.x + GLOVES_OFFSET_X, window_center.y + GLOVES_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float BOOTS_OFFSET_X = 1159.5f;
	const float BOOTS_OFFSET_Y = 342.0f;
	Rectangle boots_slot = { window_center.x + BOOTS_OFFSET_X, window_center.y + BOOTS_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float ACCESSORY_1_OFFSET_X = 1047.0f;
	const float ACCESSORY_1_OFFSET_Y = 541.5f;
	Rectangle accessory_1 = { window_center.x + ACCESSORY_1_OFFSET_X, window_center.y + ACCESSORY_1_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float ACCESSORY_2_OFFSET_X = 1158.0f;
	const float ACCESSORY_2_OFFSET_Y = 541.5f;
	Rectangle accessory_2 = { window_center.x + ACCESSORY_2_OFFSET_X, window_center.y + ACCESSORY_2_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	const float ACCESSORY_3_OFFSET_X = 1269.0f;
	const float ACCESSORY_3_OFFSET_Y = 541.5f;
	Rectangle accessory_3 = { window_center.x + ACCESSORY_3_OFFSET_X, window_center.y + ACCESSORY_3_OFFSET_Y, SLOT_SIZE, SLOT_SIZE };

	return { helm_slot, weapon_slot, shield_slot, vest_slot, gloves_slot, boots_slot, accessory_1, accessory_2, accessory_3 };
}

void handle_single_slot(item* item_ptr, Rectangle slot, Vector2 mouse_pos, player& p, item*& hovered_item)
{
	if (item_ptr != nullptr)
	{
		Texture2D* icon = item_ptr->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, slot))
		{
			hovered_item = item_ptr;
			DrawRectangleLinesEx(slot, 2.0f, GRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				item_ptr->use(&p);
				hovered_item = nullptr;
			}
		}
	}
}
void equipment_slots_handle(player& p, const equipment_slots& slots, Vector2 mouse_pos, item*& hovered_item)
{
	handle_single_slot(p.equipped_helm, slots.helm_slot, mouse_pos, p, hovered_item);
	handle_single_slot(p.equipped_weapon, slots.weapon_slot, mouse_pos, p, hovered_item);
	handle_single_slot(p.equipped_shield, slots.shield_slot, mouse_pos, p, hovered_item);
	handle_single_slot(p.equipped_vest, slots.vest_slot, mouse_pos, p, hovered_item);
	handle_single_slot(p.equipped_gauntlets, slots.gloves_slot, mouse_pos, p, hovered_item);
	handle_single_slot(p.equipped_boots, slots.boots_slot, mouse_pos, p, hovered_item);

}
