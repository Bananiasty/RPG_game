#include <filesystem>
#include <fstream>
#include "gamestates.h"
#include "graphics.h"

extern Font cabin_sketch_font;



void exploration::save_game()
{
    std::filesystem::path folder_path = "saves";
    std::filesystem::path file_path = folder_path / "save.txt";

    std::filesystem::create_directory(folder_path);

    std::ofstream plik(file_path);

    if (plik.is_open())
    {
        plik << current_floor_id << " " << current_node_id << "\n\n";

        for (size_t floor_idx = 0; floor_idx < floors.size(); ++floor_idx)
        {
            for (const auto& [id, node] : floors[floor_idx].world_map)
            {
                plik << floor_idx << " " << id << " " << node.discovered << "\n";
            }
        }

        plik.close();
    }
}

bool exploration::load_game()
{
    std::filesystem::path file_path = "saves/save.txt";

    if (!std::filesystem::exists(file_path))
    {
        return false;
    }

    std::ifstream plik(file_path);

    if (plik.is_open())
    {
        plik >> current_floor_id >> current_node_id;

        int floor_idx;
        int id;
        bool status_discovered;

        while (plik >> floor_idx >> id >> status_discovered)
        {
            if (floor_idx >= 0 && floor_idx < static_cast<int>(floors.size()))
            {
                auto& floor_map = floors[floor_idx].world_map;
                if (floor_map.find(id) != floor_map.end())
                {
                    floor_map[id].discovered = status_discovered;
                }
            }
        }

        plik.close();
        return true;
    }

    return false;
}


void gamestate::draw_menu()
{
	if (IsKeyPressed(KEY_ESCAPE))
	{
		showMenu = !showMenu;
	}

	if (!showMenu) return;

	int windowWidth = (int)(GAME_WIDTH / 3.5f); 
	int windowHeight = (int)(GAME_HEIGHT / 2.0f);
	int windowX = (int)((GAME_WIDTH - (float)windowWidth) / 2.0f);
	int windowY = (int)((GAME_HEIGHT - (float)windowHeight) / 2.0f);

	DrawRectangle(windowX, windowY, windowWidth, windowHeight, Fade(BLACK, 0.85f));
	DrawRectangleLines(windowX, windowY, windowWidth, windowHeight, RAYWHITE);

	Vector2 mouse_pos = GetVirtualMousePosition();

	int btnWidth = (int)((float)windowWidth * 0.75f);
	int btnHeight = 60;
	int btnX = windowX + (windowWidth - btnWidth) / 2;

	Rectangle loadBtnRect = { (float)btnX, (float)(windowY + windowHeight * 0.22f), (float)btnWidth, (float)btnHeight };
	bool is_load_hovered = CheckCollisionPointRec(mouse_pos, loadBtnRect);

	DrawRectangleRec(loadBtnRect, is_load_hovered ? GRAY : DARKGRAY);
	DrawRectangleLinesEx(loadBtnRect, 1, RAYWHITE);

	Vector2 loadTextSize = MeasureTextEx(cabin_sketch_font, "Wczytaj Gre", 26, 1);
	DrawTextEx(cabin_sketch_font, "Wczytaj Gre", { loadBtnRect.x + (btnWidth - loadTextSize.x) / 2, loadBtnRect.y + (btnHeight - loadTextSize.y) / 2 }, 26, 1, WHITE);

	if (is_load_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		load_game();
	}

	Rectangle saveBtnRect = { (float)btnX, (float)(windowY + windowHeight * 0.42f), (float)btnWidth, (float)btnHeight };
	Vector2 saveTextSize = MeasureTextEx(cabin_sketch_font, "Zapisz Gre", 26, 1);

	if (can_save_game)
	{
		bool is_save_hovered = CheckCollisionPointRec(mouse_pos, saveBtnRect);

		DrawRectangleRec(saveBtnRect, is_save_hovered ? GRAY : DARKGRAY);
		DrawRectangleLinesEx(saveBtnRect, 1, RAYWHITE);
		DrawTextEx(cabin_sketch_font, "Zapisz Gre", { saveBtnRect.x + (btnWidth - saveTextSize.x) / 2, saveBtnRect.y + (btnHeight - saveTextSize.y) / 2 }, 26, 1, WHITE);

		if (is_save_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			save_game();
		}
	}
	else
	{
		DrawRectangleRec(saveBtnRect, Fade(BLACK, 0.6f));
		DrawRectangleLinesEx(saveBtnRect, 1, GRAY);
		DrawTextEx(cabin_sketch_font, "Zapisz Gre", { saveBtnRect.x + (btnWidth - saveTextSize.x) / 2, saveBtnRect.y + (btnHeight - saveTextSize.y) / 2 }, 26, 1, GRAY);
	}

	Rectangle closeBtnRect = { (float)btnX, (float)(windowY + windowHeight * 0.62f), (float)btnWidth, (float)btnHeight };
	bool is_close_hovered = CheckCollisionPointRec(mouse_pos, closeBtnRect);

	DrawRectangleRec(closeBtnRect, is_close_hovered ? GRAY : DARKGRAY);
	DrawRectangleLinesEx(closeBtnRect, 1, RAYWHITE);

	Vector2 closeTextSize = MeasureTextEx(cabin_sketch_font, "Powrot", 26, 1);
	DrawTextEx(cabin_sketch_font, "Powrot", { closeBtnRect.x + (btnWidth - closeTextSize.x) / 2, closeBtnRect.y + (btnHeight - closeTextSize.y) / 2 }, 26, 1, WHITE);

	if (is_close_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		showMenu = false;
	}
}
