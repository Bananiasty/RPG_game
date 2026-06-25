#include <iostream>
#include <ctime>
#include <cstdlib>
#include <clocale>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "graphics.h"
#include "raylib.h"
#include "textureManager.h"



Font arial_font;
Font pogrubione_arial_font;

int main()
{
    setlocale(LC_CTYPE, "Polish");
    srand(time(NULL));
    player bohater("", 100, 50, 0, 100, 0, 0, 0, 0, 0, textures.player);
    exploration world{ bohater };

    gamestate* active_state = &world;

    std::string player_name = "";
    bool niepoprawny_nick = false;

    int current_state = 0;

    graphics_init();
    LoadGameTextures();

    arial_font = LoadFontEx("Fonts/arial.ttf", 32, NULL, 0);
    pogrubione_arial_font = LoadFontEx("Fonts/arialbd.ttf", 32, NULL, 0);

    battle* current_fight = nullptr;

    while (!WindowShouldClose())
    {
        global_fx.update();
        SetExitKey(KEY_NULL);
        if (current_state == 0)
        {
            int key = GetCharPressed();
            while (key > 0)
            {
                if ((key >= 32) && (key <= 125) && (player_name.length() < 12))
                {
                    player_name += (char)key;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !player_name.empty())
            {
                player_name.pop_back();
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                if (bohater.validate_and_set_name(player_name))
                {
                    current_state = 1;
                    niepoprawny_nick = false;

                }
                else
                {
                    niepoprawny_nick = true;
                }
            }

            BeginDrawing();
            draw_login_screen(player_name, niepoprawny_nick);
            EndDrawing();

            continue;
        }

        if (bohater.is_dead()) current_state = 5;

        if (current_state == 5)
        {
            BeginDrawing();
            draw_game_over();
            EndDrawing();
            continue;

        }
        if (current_state == 6)
        {
            BeginDrawing();
            draw_game_won();
            EndDrawing();
            continue;
        }

        if (current_state == 1 && world.get_current_enemy() != nullptr)
        {
            current_state = 2;
            current_fight = new battle(bohater, *world.get_current_enemy());
            bohater.xp_from_enemy_dif = world.get_current_enemy()->get_dif();

            current_fight->exp = &world;

            active_state = current_fight;
        }

        int nextID = active_state->update_state();


        if (nextID == 3 && current_state != 3)
        {
            inventory_state* inv = new inventory_state(bohater, active_state);

            inv->exp = &world;
            inv->fight = current_fight;

            active_state = inv;

            current_state = 3;
        }
        else if (nextID == -1 && current_state == 3)
        {
            inventory_state* inv = (inventory_state*)active_state;
            active_state = inv->get_previous_state();

            current_state = (active_state == &world) ? 1 : 2;

            delete inv;
        }

        if (nextID == 4 && current_state != 4)
        {
            map_state* map = new map_state(active_state);

            map->exp = &world;
            map->fight = current_fight;

            active_state = map;

            current_state = 4;
        }
        else if (current_state == 4 && ((map_state*)active_state)->back_requested)
        {
            map_state* map = (map_state*)active_state;

            active_state = map->get_previous_state();
            current_state = (active_state == &world) ? 1 : 2;

            delete map;
        }
        BeginDrawing();
        ClearBackground(BLACK);


        active_state->draw();
        if (current_state == 1 || current_state==2)
            draw_commentary();
        EndDrawing();

        if (current_state == 2)
        {
            enemy* your_opponent = world.get_current_enemy();
            
            if (your_opponent && your_opponent->is_dead())
            {
                if (your_opponent->get_name() == "Smok")
                {
                    bohater.grant_xp();
                    bohater.check_level_up();
                    delete current_fight;
                    current_fight = nullptr;
                    current_state = 6;
                }
                else
                {
                    bohater.grant_xp();
                    bohater.check_level_up();
                    world.delete_dead_enemies();
                    active_state = &world;
                    current_state = 1;
                    delete current_fight;
                    current_fight = nullptr;
                }
            }
        }
 
    }
    
    UnloadFont(arial_font);
    CloseWindow();
    return 0;
}
        
