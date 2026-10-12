#include "ui/LoginUI.h"
#include "raylib.h"
#include <string>

extern Font cabin_sketch_font;

void draw_login_screen(const std::string& current_name, bool has_error)
{
	ClearBackground(BLACK);

	DrawText("LOGOWANIE", 500, 100, 50, GOLD);
	DrawTextEx(cabin_sketch_font, "Wpisz swoj nickname (3-12 znakow, bez znakow specjalnych):", { 350, 180 }, 20, 2, GRAY);

	DrawRectangleLines(500, 230, 300, 40, LIGHTGRAY);
	DrawTextEx(cabin_sketch_font, current_name.c_str(), { 510, 240 }, 20, 3, RAYWHITE);

	DrawTextEx(cabin_sketch_font, "Wcisnij [ENTER] aby zatwierdzic", { 500, 300 }, 16, 2, DARKGRAY);

	if (has_error)
	{
		DrawTextEx(cabin_sketch_font, "BLAD: Nick zawiera znaki specjalne lub ma zla dlugosc!", { 250, 350 }, 18, 2, RED);
	}
}

void draw_game_over()
{
	ClearBackground(BLACK);
	DrawText("GAME OVER", 500, 300, 50, RED);
	DrawText("Nacisnij ESC aby wyjsc", 520, 370, 20, GRAY);
}
