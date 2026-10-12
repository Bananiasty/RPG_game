#include "ui/InventoryUI.h"
#include "ui/ItemTooltipUI.h"
#include "ui/HUD.h"
#include "ui/BattleUI.h"
#include "textureManager.h"
#include "graphics.h"
#include "gamestates.h"

void draw_inventory_ui(player& p, inventory_state* inv)
{
	Texture2D inventory = textures.inventory_UI;
	Texture2D item_icon;

	Vector2 mouse_pos = GetVirtualMousePosition();
	Vector2 window_center = { (float)(GAME_WIDTH * 0.5f - inventory.width * 0.5f), (float)(GAME_HEIGHT * 0.5f - inventory.height * 0.5f) };

	item* hovered_item = nullptr;

	Rectangle equipment_button = { window_center.x + inventory.width * 0.057f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle item_button = { window_center.x + inventory.width * 0.181f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle food_button = { window_center.x + inventory.width * 0.308f, window_center.y + inventory.height * 0.081f, 120, 120 };
	Rectangle books_button = { window_center.x + inventory.width * 0.437f, window_center.y + inventory.height * 0.081f, 120, 120 };

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

	DrawTextureEx(inventory, window_center, 0.0f, 1.0f, WHITE);
	draw_player_stats(p);
	if (p.equipped_helm != nullptr)
	{
		Texture2D* icon = p.equipped_helm->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, helm_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, helm_slot))
		{
			hovered_item = p.equipped_helm;
			DrawRectangleLinesEx(helm_slot, 2.0f, GRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_helm->use(&p);
				hovered_item = nullptr;
			}
		}
	}

	if (p.equipped_weapon != nullptr)
	{
		Texture2D* icon = p.equipped_weapon->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, weapon_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, weapon_slot))
		{
			hovered_item = p.equipped_weapon;
			DrawRectangleLinesEx(weapon_slot, 2.0f, GRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_weapon->use(&p);
				hovered_item = nullptr;
			}
		}
	}

	if (p.equipped_shield != nullptr)
	{
		Texture2D* icon = p.equipped_shield->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, shield_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, shield_slot))
		{
			hovered_item = p.equipped_shield;
			DrawRectangleLinesEx(shield_slot, 2.0f, GRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_shield->use(&p);
				hovered_item = nullptr;
			}
		}
	}

	if (p.equipped_vest != nullptr)
	{
		Texture2D* icon = p.equipped_vest->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, vest_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, vest_slot))
		{
			hovered_item = p.equipped_vest;
			DrawRectangleLinesEx(vest_slot, 2.0f, GRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_vest->use(&p);
				hovered_item = nullptr;
			}
		}
	}

	if (p.equipped_gauntlets != nullptr)
	{
		Texture2D* icon = p.equipped_gauntlets->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, gloves_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, gloves_slot))
		{
			hovered_item = p.equipped_gauntlets;
			DrawRectangleLinesEx(gloves_slot, 2.0f, GRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_gauntlets->use(&p);
				hovered_item = nullptr;
			}
		}
	}

	if (p.equipped_boots != nullptr)
	{
		Texture2D* icon = p.equipped_boots->icon_texture;
		Rectangle source_rec = { 0.0f, 0.0f, (float)icon->width, (float)icon->height };
		DrawTexturePro(*icon, source_rec, boots_slot, { 0.0f, 0.0f }, 0.0f, WHITE);

		if (CheckCollisionPointRec(mouse_pos, boots_slot))
		{
			hovered_item = p.equipped_boots;
			DrawRectangleLinesEx(boots_slot, 2.0f, GRAY);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				p.equipped_boots->use(&p);
				hovered_item = nullptr;
			}
		}
	}

	Vector2 body_pos = { GAME_WIDTH * 0.78f, GAME_HEIGHT * 0.15f };
	body_UI.draw(p, body_pos, 2.0f);

	bool equipment_button_isHovered = CheckCollisionPointRec(mouse_pos, equipment_button);
	bool item_button_isHovered = CheckCollisionPointRec(mouse_pos, item_button);
	bool food_button_isHovered = CheckCollisionPointRec(mouse_pos, food_button);
	bool books_button_isHovered = CheckCollisionPointRec(mouse_pos, books_button);

	p.sort_bag();

	if (equipment_button_isHovered)
	{
		DrawRectangleLinesEx(equipment_button, 5, WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			inv->equipment_tab = true;
			inv->items_tab = false;
			inv->food_tab = false;
			inv->books_tab = false;
		}
	}
	if (item_button_isHovered)
	{
		DrawRectangleLinesEx(item_button, 5, WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			inv->equipment_tab = false;
			inv->items_tab = true;
			inv->food_tab = false;
			inv->books_tab = false;
		}
	}
	if (food_button_isHovered)
	{
		DrawRectangleLinesEx(food_button, 5, WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			inv->equipment_tab = false;
			inv->items_tab = false;
			inv->food_tab = true;
			inv->books_tab = false;
		}
	}
	if (books_button_isHovered)
	{
		DrawRectangleLinesEx(books_button, 5, WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			inv->equipment_tab = false;
			inv->items_tab = false;
			inv->food_tab = false;
			inv->books_tab = true;
		}
	}
	auto& all_items = p.bag->items;

	auto& equipment = p.equipment->items;
	auto& other_items = p.inv_items->items;
	auto& food = p.food->items;
	auto& books = p.books->items;

	int startX = 80;
	int startY = 250;
	int btnWidth = 60;
	int btnHeight = 60;
	int bottom_padding = 15;
	int right_padding = 140;

	bool is_item_hovered;
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
			is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

			if (is_item_hovered)
			{
				if (dynamic_cast<helm*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(helm_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(helm_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<weapon*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(weapon_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(weapon_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<shield*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(shield_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(shield_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<vest*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(vest_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(vest_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<gauntlets*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(gloves_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(gloves_slot, 2.5f, WHITE);
				}
				else if (dynamic_cast<boots*>(equipment[i]) != nullptr)
				{
					DrawRectangleRec(boots_slot, Fade(GRAY, 0.35f));
					DrawRectangleLinesEx(boots_slot, 2.5f, WHITE);
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
	else if (inv->items_tab == true)
	{
		for (int i = 0; i < other_items.size(); i++)
		{
			item_icon = *(other_items[i]->icon_texture);

			int row = i / 5;
			int col = i % 5;
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
					other_items[i]->use(&p);
					p.sort_bag();
					hovered_item = nullptr;
					break;
				}
				if (IsKeyPressed(KEY_D))
				{
					inv->pending_drops.push_back(other_items[i]);
					p.drop_item(other_items[i]);
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
				hovered_item = other_items[i];
				DrawRectangleLinesEx(itemRect, 2.0f, RAYWHITE);
			}
		}
	}
	else if (inv->food_tab == true)
	{
		for (int i = 0; i < food.size(); i++)
		{
			item_icon = *(food[i]->icon_texture);

			int row = i / 5;
			int col = i % 5;
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
					food[i]->use(&p);
					p.sort_bag();
					hovered_item = nullptr;
					break;
				}
				if (IsKeyPressed(KEY_D))
				{
					inv->pending_drops.push_back(food[i]);
					p.drop_item(food[i]);
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
				hovered_item = food[i];
				DrawRectangleLinesEx(itemRect, 2.0f, RAYWHITE);
			}
		}
	}
	else if (inv->books_tab == true)
	{
		for (int i = 0; i < books.size(); i++)
		{
			item_icon = *(books[i]->icon_texture);

			int row = i / 5;
			int col = i % 5;
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
					books[i]->use(&p);
					p.sort_bag();
					hovered_item = nullptr;
					break;
				}
				if (IsKeyPressed(KEY_D))
				{
					inv->pending_drops.push_back(books[i]);
					p.drop_item(books[i]);
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
				hovered_item = books[i];
				DrawRectangleLinesEx(itemRect, 2.0f, RAYWHITE);
			}
		}
	}

	if (hovered_item != nullptr)
	{
		draw_item_tooltip(hovered_item, mouse_pos);
	}
}
