#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "graphics.h"
#include "character.h"
#include "inventory_class.h"
#include "textureManager.h"
#include <ranges>

extern Font arial_font;
extern Font pogrubione_arial_font;

void draw_login_screen(const std::string& current_name, bool has_error)
{
	ClearBackground(BLACK);

	DrawText( "LOGOWANIE", 500, 100, 50, GOLD);
	DrawTextEx(arial_font, "Wpisz swoj nickname (3-12 znakow, bez znakow specjalnych):", { 350, 180 }, 20, 2, GRAY);

	DrawRectangleLines(500, 230, 300, 40, LIGHTGRAY);
	DrawTextEx(arial_font, current_name.c_str(), { 510, 240 }, 20, 3, RAYWHITE);

	DrawTextEx(arial_font, "Wcisnij [ENTER] aby zatwierdzic", { 500, 300 }, 16, 2, DARKGRAY);

	if (has_error)
	{
		DrawTextEx(arial_font, "BLAD: Nick zawiera znaki specjalne lub ma zla dlugosc!", { 250, 350 }, 18, 2, RED);
	}
}

void draw_game_scene(exploration* exp)
{

	DrawTextureEx(textures.kopalnia, { 0,0 }, 0, 0.32, WHITE);
	
	DrawTextEx(arial_font, TextFormat("LOKACJA: %s", exp->get_current_node()->description.c_str()), { 20, 20 }, 20, 2, LIGHTGRAY);
	
	DrawRectangle(1000, 0, 280, 400, DARKPURPLE);
	DrawTextEx(pogrubione_arial_font, "STATYSTYKI GRACZA", { 1010, 50 }, 25, 2, GOLD);
	DrawTextEx(arial_font, TextFormat("ATAK: %d", exp->bohater.get_damage()), { 1015, 100 }, 20, 2, YELLOW);
	DrawTextEx(arial_font, TextFormat("OBRONA: %d", exp->bohater.get_defense()), { 1015, 130 }, 20, 2, YELLOW);

	DrawTextEx(arial_font, "KRYT:", { 1015, 210 }, 20, 2, YELLOW);
	DrawTextEx(arial_font, TextFormat("%d%%", exp->bohater.get_crit_chance()), { 1090, 210 }, 20, 2, YELLOW);

	DrawTextEx(arial_font, TextFormat("BLOK: %d%%", exp->bohater.get_block_chance()), { 1015, 240 }, 20, 2, YELLOW);
	DrawTextEx(arial_font, TextFormat("UNIK: %d%%", exp->bohater.get_dodge_chance()), { 1015, 270 }, 20, 2, YELLOW);

	
	float szerokosc_tekstu = (float)MeasureText(exp->bohater.get_name().c_str(), 24);
	float posX = 270 - (szerokosc_tekstu / 2.0);
	DrawTextEx(pogrubione_arial_font, exp->bohater.get_name().c_str(), { posX, 360 }, 24, 2, RAYWHITE);


	float centerX = 50.0 + (((float)textures.player.width * 0.17) / 2.0);
	float centerY = 100.0 + (((float)textures.player.height * 0.17) / 2.0);
	
	DrawCircleGradient({ centerX, centerY }, 300, Color{ 100, 200, 255, 120 }, BLANK);

	DrawTextureEx(textures.player, { 50, 100 }, 0, 0.17, WHITE);


	DrawRectangle(0, 450, 1280, 270, BLACK);
	DrawRectangle(0, 400, 1000, 80, Color{ 245, 245, 220, 255 });
	DrawRectangleLinesEx({ 0, 400, 1000, 80 }, 5, Color{ 133, 94, 66, 255 });

	DrawTextEx(arial_font, TextFormat("HP Bohatera: %d", exp->bohater.get_health()), { 50, 620 }, 20, 2, MAROON);
	DrawRectangle(50, 650, 300, 30, DARKGRAY);
	float hp_width = (float)exp->bohater.get_health() / exp->bohater.get_max_health() * 300;
	DrawRectangle(50, 650, (int)hp_width, 30, RED);


}
//RANGES
void draw_commentary()
{
	if (!gamestate::gameLogs.empty())
	{
		float przesuniecie_y = 0.0;
		for (const auto& log : gamestate::gameLogs | std::views::reverse | std::views::take(3))
		{
			DrawTextEx(arial_font, log.c_str(), { 50, 410 + przesuniecie_y }, 25, 2, PINK);
			przesuniecie_y += 20.0;
		}		
	}
}

void draw_map(map_state* map, exploration* exp)
{
	Node* current_Node = exp->get_current_node();

	DrawRectangle(100, 100, 1080, 520, Fade(BLACK, 0.8));
	DrawRectangleLines(100, 100, 1080, 520, RAYWHITE);
		
		float posX = current_Node->positionX;
		float posY = current_Node->positionY;

		auto i = exp->world_map.begin();

		while (i != exp->world_map.end())
		{			
			float posX = i->second.positionX;
			float posY = i->second.positionY;
			if (i->first != -1)
			{
				if(i->second.discovered)
				{
					DrawRectangle(posX, posY, 60, 60, DARKGREEN);
					DrawRectangleLinesEx({ posX, posY, 60, 60 }, 3, RAYWHITE);
				}
				if (i->first == exp->current_node_id)
				{
					DrawRectangle(posX, posY, 60, 60, GREEN);
					DrawRectangleLinesEx({ posX, posY, 60, 60 }, 3, RAYWHITE);
				}
				if (i->second.previous_id!=-1 && i->second.discovered)
				{
					Node& parent = exp->world_map[i->second.previous_id];
					float parent_posX = parent.positionX;
					float parent_posY = parent.positionY;
					DrawLineEx({ posX + 30, posY + 30 }, { parent_posX + 30, parent_posY + 30 }, 5, WHITE);
				}
			}

			i++;
		}
		
		if (GuiButton({ 1000,100,100,150 }, "Powrot")) {
			map->back_requested = true;
		}
}

void draw_buttons(exploration* e) 
{
	GuiSetFont(arial_font);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
	Node* current = e->get_current_node();
	if (current->spawn_chest != nullptr) {
		
		DrawText("ZNALAZLES  SKRZYNIE!", 400, 200, 30, GOLD);

		if (GuiButton({ 400, 300, 200, 50 }, "WEZ  WSZYSTKO")) {
			e->collect_loot();
		}

		
		if (GuiButton({ 400, 360, 200, 50 }, "ZOSTAW")) {
			e->discard_chest();
		}
	}
	if (GuiButton({ 1000, 350, 280, 150 }, "EKWIPUNEK")) {
		e->showInventory = true;
	}
	if (GuiButton({ 1000, 550, 280, 150 }, "MAPA")) {
		e->showMap = true;
	}
	Rectangle btnLeft = { 400, 520, 200, 70 };
	Rectangle btnRight = { 750, 520, 200, 70 };
	Rectangle btnBack = { 50, 520, 200, 70 };
	

	if (current->left_id != -1) {
		if (GuiButton(btnLeft, "Lewo")) {
			e->move_left();
		}
	}

	if (current->right_id != -1) {
		if (GuiButton(btnRight, "Prawo")) {
			e->move_right();
		}
	}

	if (current->previous_id != -1) {
		if (GuiButton(btnBack, "Powrot")) {
			e->move_back();
		}
	}
	
}
void draw_inventory_ui(player& p, inventory_state* inv) {
	
	DrawRectangle(100, 100, 1080, 520, Fade(BLACK, 0.8));
	DrawRectangleLines(100, 100, 1080, 520, RAYWHITE);

	
	auto& items = p.bag->items;

	if (items.empty()) {
		DrawTextEx(arial_font, "Twoj plecak jest pusty...", { 500, 300 }, 20, 2, GRAY);
	}
	else {
		
		int startX = 140;
		int startY = 180;
		int btnWidth = 180;
		int btnHeight = 60;
		int padding = 15;

		for (int i = 0; i < items.size(); i++) {
			
			int row = i / 5;
			int col = i % 5;
			Rectangle itemRect = {
				(float)(startX + col * (btnWidth + padding)),
				(float)(startY + row * (btnHeight + padding)),
				(float)btnWidth,
				(float)btnHeight
			};

			
			std::string label = items[i]->get_name();
			if (items[i]->is_equipped()) {
				label = "[E] " + label;
			}

			
			if (GuiButton(itemRect, label.c_str())) {
				items[i]->use(&p, i);
			}
			
		}
	}
	if (GuiButton({ 1000,100,200,100 }, "Powrot")) {
		inv->request_back();
	}
}



void draw_battle_ui(battle* fight) {

	std::string e = fight->get_enemy_name();
	float hp_width = (float)fight->get_enemy_hp() / fight->get_enemy_max_hp() * 300;
	DrawRectangle(600, 650, 300, 30, DARKGRAY);
	DrawRectangle(600, 650, (int)hp_width, 30, RED);
	DrawTextEx(arial_font, TextFormat("HP Wroga: %d", fight->get_enemy_hp()), { 600, 620 }, 20, 2, MAROON);

		Texture2D enemy_texture;

		float scale = 0.17;
		if (e == "Szkielet") enemy_texture = textures.skeleton;
		else if (e == "Troll")  enemy_texture = textures.troll;
		else if (e == "Bandyci") enemy_texture = textures.bandits;
		else if (e == "Straznik") enemy_texture = textures.guard;
		else if (e == "Smok") enemy_texture = textures.dragon;
		else if (e == "Goblin")
		{
			enemy_texture = textures.goblin;
			scale = 0.47;
		}
		
		float przeskalowana_szerokosc_wroga = (float)enemy_texture.width * scale;
		float posX = 670.0 - (przeskalowana_szerokosc_wroga / 2.0);
		float posY = 55.0;

		float centerX = 430.0 + (((float)enemy_texture.width * 0.17) / 2.0);
		float centerY = 80.0 + (((float)enemy_texture.height * 0.17) / 2.0);
		if (e != "Goblin")
		{
			DrawCircleGradient({ centerX, centerY }, 300, Color{ 255, 0, 0, 60 }, BLANK);
		}
		else
		{
			DrawCircleGradient({ 650, 200 }, 200, Color{ 255, 0, 0, 60 }, BLANK);
		}
		
		DrawTextureEx(enemy_texture, { posX, posY }, 0, scale, WHITE);

		float szerokosc_tekstu_e = (float)MeasureText(e.c_str(), 24);
		float posX_text = 675 - (szerokosc_tekstu_e / 2);

		DrawTextEx(pogrubione_arial_font, e.c_str(), { posX_text, 320 }, 24, 2, RED);
		
		if (!fight->showInventory) {
			
			if (GuiButton({ 50, 500, 250, 50 }, "SZYBKI ATAK")) {
				
				if (fight->player_cooldown <= 0)
				{
					fight->attack_clicked = true;
					fight->player_cooldown = 2.5;
				}
					
			}
			if (GuiButton({ 1000, 350, 280, 150 }, "EKWIPUNEK")) {
				fight->showInventory = true;
			}
			if (GuiButton({ 1000, 550, 280, 150 }, "MAPA")) {
				fight->showMap = true;
			}
		}
	
}

void gamestate::draw_menu()
{
	if (IsKeyPressed(KEY_ESCAPE))
	{ 
		showMenu = !showMenu;
	}

	if (showMenu)
	{
		DrawRectangle(350, 100, 580, 550, Fade(BLACK, 0.8));
		DrawRectangleLines(350, 100, 580, 550, LIGHTGRAY);
		if (!can_save_game)
		{
			GuiDisable();
		}
		if (GuiButton({ 550, 325, 200, 80 }, "Zapisz Gre")) {
			save_game();
		}
		if (GuiButton({ 550, 200, 200, 80 }, "Wczytaj Gre")) {
			load_game();
		}
		if (!can_save_game)
		{
			GuiEnable();
		}
	}
	
		
}

	
	

void graphics_init()
{
	const int screenWidth = 1280;
	const int screenHeight = 720;
	InitWindow(screenWidth, screenHeight, "textRPG");
	SetTargetFPS(60);

}

void draw_game_over() 
{
	ClearBackground(BLACK);
	DrawText("GAME OVER", 500, 300, 50, RED);
	DrawText("Nacisnij ESC aby wyjsc", 520, 370, 20, GRAY);
}

void draw_game_won()
{
	ClearBackground(GOLD);
	DrawText("WYGRALES", 500, 300, 50, RED);
}


