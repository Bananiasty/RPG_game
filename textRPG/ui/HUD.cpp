#include "ui/HUD.h"
#include "graphics.h"
#include "entities/Player.h"
#include "gamestates.h"
#include "textureManager.h"
#include <ranges>

extern Font cabin_sketch_font;

void DrawHUD(exploration* exp)
{
	const player& p = exp->bohater;

	float start_x = GAME_WIDTH * 0.90f;
	float start_y = GAME_HEIGHT * 0.55f;
	int icon_idx = 0;

	for (const auto& effect : p.get_effects())
	{
		Texture2D icon = effect->get_icon();
		if (icon.id > 0)
		{
			Vector2 pos = { start_x, start_y + (icon_idx * 50.0f) };
			DrawTextureEx(icon, pos, 0.0f, 2.5f, WHITE);
			DrawText(TextFormat("%d", effect->get_remaining_turns()), (int)pos.x + 36, (int)pos.y + 24, 18, YELLOW);
			icon_idx++;
		}
	}
	if (icon_idx == 0 && p.is_bleeding)
	{
		Texture2D bleed = textures.bleeding_icon;
		Vector2 pos = { start_x, start_y };
		DrawTextureEx(bleed, pos, 0.0f, 2.5f, WHITE);
	}
	DrawRectangle((int)(GAME_WIDTH * 0.8f), (int)(GAME_HEIGHT * 0.9f), 50, 20, WHITE);
	DrawText("I", (int)(GAME_WIDTH * 0.8f + 20.0f), (int)(GAME_HEIGHT * 0.9f), 20, DARKBROWN);

	DrawRectangle((int)(GAME_WIDTH * 0.7f), (int)(GAME_HEIGHT * 0.9f), 50, 20, WHITE);
	DrawText("M", (int)(GAME_WIDTH * 0.7f + 20.0f), (int)(GAME_HEIGHT * 0.9f), 20, DARKBROWN);

	if (exp->active_ui_event == nullptr && exp->is_hovering_interactive)
	{
		const char* prompt = "Press [E]";
		int font_size = 44;
		int text_width = MeasureText(prompt, font_size);

		int text_x = (int)(GAME_WIDTH / 2.0f) - (text_width / 2);
		int text_y = (int)(GAME_HEIGHT / 2.0f) - 50;

		DrawTextEx(cabin_sketch_font, prompt, { (float)text_x + 1.0f, (float)text_y + 1.0f }, (float)font_size, 1.0f, BLACK);
		DrawTextEx(cabin_sketch_font, prompt, { (float)text_x, (float)text_y }, (float)font_size, 1.0f, YELLOW);
	}
}

void draw_player_stats(player& p)
{
	float x = (GAME_WIDTH * 0.58f);
	float y = (GAME_HEIGHT * 0.73f);

	int offset_x = 200;
	int offset_y = 40;
	DrawTextEx(cabin_sketch_font, TextFormat("HP: %d/%d", p.get_health(), p.get_max_health()), { x, y + offset_y }, 38, 2, WHITE);
	DrawTextEx(cabin_sketch_font, TextFormat("Armor: %d", p.get_defense()), { x, y + offset_y * 2 }, 38, 2, WHITE);
	DrawTextEx(cabin_sketch_font, TextFormat("Block Chance: %d%%", p.get_block_chance()), { x + offset_x, y }, 38, 2, WHITE);
	DrawTextEx(cabin_sketch_font, TextFormat("Crit Chance: %d%%", p.get_crit_chance()), { x + offset_x, y + offset_y }, 38, 2, WHITE);
	DrawTextEx(cabin_sketch_font, TextFormat("Dodge Chance: %d%%", p.get_dodge_chance()), { x + offset_x, y + offset_y * 2 }, 38, 2, WHITE);
}

void draw_commentary()
{
	if (!gamestate::gameLogs.empty())
	{
		float przesuniecie_y = 0.0f;
		for (const auto& log : gamestate::gameLogs | std::views::reverse | std::views::take(3))
		{
			DrawTextEx(cabin_sketch_font, log.c_str(), { 50, 410 + przesuniecie_y }, 40, 2, WHITE);
			przesuniecie_y += 70.0f;
		}
	}
}
