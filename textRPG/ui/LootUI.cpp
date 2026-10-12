#include "ui/LootUI.h"
#include "ui/ItemTooltipUI.h"
#include "graphics.h"
#include "struct.h"
#include "gamestates.h"
#include <vector>

extern Font cabin_sketch_font;

bool draw_drop(exploration* exp, object* current_drop, bool& is_open)
{
	int window_w = (int)(GAME_WIDTH / 2.0f);
	int window_h = (int)(GAME_HEIGHT / 2.0f);
	int window_x_start = (int)(GAME_WIDTH / 2.0f - window_w / 2.0f); 
	int windows_y_start = (int)(GAME_HEIGHT / 2.0f - window_h / 2.0f);

	Texture2D item_icon;
	Vector2 mouse_pos = GetVirtualMousePosition();

	if (current_drop == nullptr || !is_open)
	{
		return false;
	}

	loot_object* drop = dynamic_cast<loot_object*>(current_drop);
	if (drop == nullptr) return false;

	std::vector<item*> raw_loot;
	raw_loot.reserve(drop->drop_loot.size());
	for (auto& up : drop->drop_loot) raw_loot.push_back(up.get());

	if (raw_loot.empty())
	{
		is_open = false;
		return true;
	}

	DrawRectangle(window_x_start, windows_y_start, window_w, window_h, Fade(BLACK, 0.85f));
	DrawRectangleLines(window_x_start, windows_y_start, window_w, window_h, RAYWHITE);

	Rectangle closeBtnRect = {
		(float)(window_x_start + window_w - 55),
		(float)(windows_y_start + 15),
		40.0f,
		40.0f
	};
	bool is_close_hovered = CheckCollisionPointRec(mouse_pos, closeBtnRect);

	DrawRectangleRec(closeBtnRect, is_close_hovered ? RED : MAROON);
	DrawRectangleLinesEx(closeBtnRect, 1, RAYWHITE);
	DrawTextEx(cabin_sketch_font, "X", { closeBtnRect.x + 12, closeBtnRect.y + 5 }, 28, 1, WHITE);

	if (is_close_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		is_open = false;
		return true;
	}

	Rectangle takeAllRect = {
		(float)(window_x_start + (window_w - 210) / 2),
		(float)(windows_y_start + window_h - 100),
		200.0f,
		80.0f
	};
	bool is_take_all_hovered = CheckCollisionPointRec(mouse_pos, takeAllRect);

	DrawRectangleRec(takeAllRect, is_take_all_hovered ? DARKGRAY : DARKGREEN);
	DrawRectangleLinesEx(takeAllRect, 1, RAYWHITE);
	DrawTextEx(cabin_sketch_font, "Take all", { takeAllRect.x + 12, takeAllRect.y + 20 }, 40, 1, WHITE);

	if (is_take_all_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		for (auto& up : drop->drop_loot)
		{
			if (up)
			{
				exp->bohater.bag->add_item(up.release());
			}
		}
		drop->drop_loot.clear();
		exp->bohater.sort_bag();
		is_open = false;
		return true;
	}

	int startX = window_x_start + 60;
	int startY = windows_y_start + 80;
	int btnWidth = 60;
	int btnHeight = 60;
	int padding = 25;

	int clicked_index = -1;
	item* drop_hovered_item = nullptr;

	for (int i = 0; i < (int)raw_loot.size(); i++)
	{
		if (raw_loot[i] == nullptr)
		{
			continue;
		}

		item_icon = *raw_loot[i]->icon_texture;

		int row = i / 6;
		int col = i % 6;

		Rectangle itemRect = {
			(float)(startX + col * (btnWidth + padding)),
			(float)(startY + row * (btnHeight + padding)),
			(float)btnWidth,
			(float)btnHeight
		};

		bool is_item_hovered = CheckCollisionPointRec(mouse_pos, itemRect);

		DrawRectangleRec(itemRect, is_item_hovered ? GRAY : DARKGRAY);
		DrawRectangleLinesEx(itemRect, 1, RAYWHITE);

		if (is_item_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			clicked_index = i;
			break;
		}

		if (item_icon.id > 0)
		{
			float scale = (float)(btnHeight - 10) / item_icon.height;
			DrawTextureEx(item_icon, { itemRect.x + 5, itemRect.y + 5 }, 0.0f, scale, WHITE);
		}

		if (is_item_hovered)
		{
			drop_hovered_item = raw_loot[i];
		}
	}

	if (clicked_index != -1)
	{
		// transfer ownership of the clicked item into player's bag
		if (clicked_index >= 0 && clicked_index < (int)drop->drop_loot.size())
		{
			auto it = drop->drop_loot.begin() + clicked_index;
			if (it != drop->drop_loot.end() && *it)
			{
				item* raw = it->release();
				drop->drop_loot.erase(it);
				exp->bohater.bag->add_item(raw);
			}
		}

		if (drop->drop_loot.empty())
		{
			is_open = false;
			return true;
		}
	}

	if (drop_hovered_item != nullptr && clicked_index == -1)
	{
		draw_item_tooltip(drop_hovered_item, mouse_pos);
	}

	return false;
}
