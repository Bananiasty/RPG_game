#include "ui/ItemTooltipUI.h"
#include "inventory/Item.h"
#include "inventory/Equipment.h"
#include "inventory/Consumable.h"
#include "inventory/Scroll.h"
#include "graphics.h"
#include <vector>
#include <string>

extern Font cabin_sketch_font;
extern Font cabin_sketch_font_bold;

void draw_item_tooltip(item* it, Vector2 mouse_pos)
{
	if (it == nullptr) return;

	std::string name = it->get_name();
	std::string rarity = it->get_rarity();

	// Determine item type subtitle
	std::string item_type_str = "";
	weapon* w = dynamic_cast<weapon*>(it);
	shield* s = dynamic_cast<shield*>(it);
	helm* h = dynamic_cast<helm*>(it);
	vest* v = dynamic_cast<vest*>(it);
	gauntlets* g = dynamic_cast<gauntlets*>(it);
	boots* b = dynamic_cast<boots*>(it);
	combat_scroll* cs = dynamic_cast<combat_scroll*>(it);
	health_potion* hp = dynamic_cast<health_potion*>(it);

	if (w != nullptr) item_type_str = "Weapon";
	else if (s != nullptr) item_type_str = "Shield";
	else if (h != nullptr) item_type_str = "Helmet";
	else if (v != nullptr) item_type_str = "Chest Armor";
	else if (g != nullptr) item_type_str = "Gauntlets";
	else if (b != nullptr) item_type_str = "Boots";
	else if (cs != nullptr) item_type_str = "Combat Scroll";
	else if (hp != nullptr) item_type_str = "Potion";
	else if (it->get_type() == item_type::ARMOR) item_type_str = "Armor";
	else if (it->get_type() == item_type::FOOD) item_type_str = "Food";
	else if (it->get_type() == item_type::BOOK) item_type_str = "Scroll";
	else if (it->get_type() == item_type::ITEM) item_type_str = "Item";

	std::string subtitle = "";
	if (!rarity.empty() && !item_type_str.empty())
	{
		subtitle = rarity + " " + item_type_str;
	}
	else if (!rarity.empty())
	{
		subtitle = rarity;
	}
	else if (!item_type_str.empty())
	{
		subtitle = item_type_str;
	}

	// Title color based on rarity
	Color title_color = Color{ 245, 245, 250, 255 }; // Default Raywhite
	Color sub_color = Color{ 160, 160, 175, 255 };
	if (rarity == "Rare" || rarity == "rare")
	{
		title_color = Color{ 255, 215, 0, 255 }; // Gold
		sub_color = Color{ 230, 200, 80, 255 };
	}
	else if (rarity == "Epic" || rarity == "epic")
	{
		title_color = Color{ 190, 120, 255, 255 }; // Purple
		sub_color = Color{ 180, 140, 230, 255 };
	}
	else if (rarity == "Legendary" || rarity == "legendary")
	{
		title_color = Color{ 255, 145, 40, 255 }; // Orange
		sub_color = Color{ 240, 160, 70, 255 };
	}

	struct StatEntry {
		std::string text;
		Color color;
	};
	std::vector<StatEntry> stats;

	// 1. Damage
	int atk = it->get_attack_stat();
	if (atk > 0)
	{
		stats.push_back({ TextFormat("Damage: +%d", atk), Color{ 255, 120, 110, 255 } });
	}

	// 2. Armor
	int def = it->get_defense_stat();
	if (def > 0)
	{
		stats.push_back({ TextFormat("Armor: +%d", def), Color{ 120, 195, 255, 255 } });
	}

	// 3. Specific equipment stats
	if (s != nullptr && s->get_block_chance() > 0)
	{
		stats.push_back({ TextFormat("Block Chance: +%d%%", s->get_block_chance()), Color{ 130, 230, 190, 255 } });
	}
	if (v != nullptr && v->get_add_health() > 0)
	{
		stats.push_back({ TextFormat("Max HP: +%d", v->get_add_health()), Color{ 120, 240, 130, 255 } });
	}
	if (g != nullptr && g->get_crit_chance_mod() > 0)
	{
		stats.push_back({ TextFormat("Crit Chance: +%d%%", g->get_crit_chance_mod()), Color{ 255, 185, 90, 255 } });
	}
	if (b != nullptr && b->get_dodge_chance_mod() > 0)
	{
		stats.push_back({ TextFormat("Dodge Chance: +%d%%", b->get_dodge_chance_mod()), Color{ 215, 175, 255, 255 } });
	}

	// 4. Combat scroll damage
	if (cs != nullptr && cs->get_damage() > 0.0f)
	{
		stats.push_back({ TextFormat("Spell Damage: %.0f", cs->get_damage()), Color{ 255, 140, 90, 255 } });
	}

	// 5. Health restore
	int restore_hp = it->get_restore_health();
	if (restore_hp > 0)
	{
		stats.push_back({ TextFormat("Restores HP: +%d", restore_hp), Color{ 120, 240, 130, 255 } });
	}

	// 6. Value
	int cost = it->get_cost();
	if (cost > 0)
	{
		stats.push_back({ TextFormat("Value: %d Gold", cost), Color{ 245, 215, 110, 255 } });
	}

	// Dynamic measurements
	const float TITLE_FONT_SIZE = 38.0f;
	const float SUB_FONT_SIZE = 25.0f;
	const float STAT_FONT_SIZE = 30.0f;
	const float SPACING = 1.5f;

	Vector2 title_dim = MeasureTextEx(cabin_sketch_font_bold, name.c_str(), TITLE_FONT_SIZE, SPACING);
	Vector2 sub_dim = !subtitle.empty() ? MeasureTextEx(cabin_sketch_font, subtitle.c_str(), SUB_FONT_SIZE, SPACING) : Vector2{ 0.0f, 0.0f };

	float max_text_width = title_dim.x;
	if (sub_dim.x > max_text_width) max_text_width = sub_dim.x;

	for (const auto& entry : stats)
	{
		Vector2 stat_dim = MeasureTextEx(cabin_sketch_font, entry.text.c_str(), STAT_FONT_SIZE, SPACING);
		if (stat_dim.x > max_text_width) max_text_width = stat_dim.x;
	}

	const float PAD_X = 16.0f;
	const float PAD_Y = 12.0f;
	const float TITLE_LINE_H = 26.0f;
	const float SUB_LINE_H = 19.0f;
	const float STAT_LINE_H = 22.0f;
	const float SEP_H = 10.0f;

	float box_width = max_text_width + PAD_X * 2.0f;
	if (box_width < 170.0f) box_width = 170.0f;

	float box_height = PAD_Y + TITLE_LINE_H;
	if (!subtitle.empty())
	{
		box_height += SUB_LINE_H;
	}
	if (!stats.empty())
	{
		box_height += SEP_H + (float)stats.size() * STAT_LINE_H;
	}
	box_height += PAD_Y;

	// Positioning: offset from cursor, clamp within virtual screen
	float box_x = mouse_pos.x + 18.0f;
	float box_y = mouse_pos.y + 18.0f;

	if (box_x + box_width > GAME_WIDTH - 15.0f)
	{
		box_x = mouse_pos.x - box_width - 12.0f;
	}
	if (box_x < 15.0f)
	{
		box_x = 15.0f;
	}

	if (box_y + box_height > GAME_HEIGHT - 15.0f)
	{
		box_y = mouse_pos.y - box_height - 12.0f;
	}
	if (box_y < 15.0f)
	{
		box_y = 15.0f;
	}

	// 1. Draw tooltip background
	Rectangle box_rec = { box_x, box_y, box_width, box_height };
	DrawRectangleRec(box_rec, Color{ 14, 14, 18, 235 });
	DrawRectangleLinesEx(box_rec, 1.5f, Color{ 140, 140, 155, 255 });

	// 2. Draw Title
	float cur_y = box_y + PAD_Y;
	float text_x = box_x + PAD_X;
	DrawTextEx(cabin_sketch_font_bold, name.c_str(), { text_x, cur_y }, TITLE_FONT_SIZE, SPACING, title_color);
	cur_y += TITLE_LINE_H;

	// 3. Draw Subtitle
	if (!subtitle.empty())
	{
		DrawTextEx(cabin_sketch_font, subtitle.c_str(), { text_x, cur_y }, SUB_FONT_SIZE, SPACING, sub_color);
		cur_y += SUB_LINE_H;
	}

	// 4. Draw separator and stats
	if (!stats.empty())
	{
		cur_y += 3.0f;
		DrawLineEx({ box_x + 10.0f, cur_y }, { box_x + box_width - 10.0f, cur_y }, 1.0f, Color{ 75, 75, 90, 200 });
		cur_y += 7.0f;

		for (const auto& entry : stats)
		{
			DrawTextEx(cabin_sketch_font, entry.text.c_str(), { text_x, cur_y }, STAT_FONT_SIZE, SPACING, entry.color);
			cur_y += STAT_LINE_H;
		}
	}
}
