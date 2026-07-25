#include <iostream>
#include <ctime>
#include <cstdlib>
#include <clocale>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "graphics.h"
#include "raygui.h"
#include "raylib.h"
#include "raymath.h"
#include "textureManager.h"



Font arial_font;
Font pogrubione_arial_font;

int main()
{
    setlocale(LC_CTYPE, "Polish");
    srand(time(NULL));
    player bohater("", 100, 50, 0, 10, 0, 0, 0, 0, 1, textures.player, { 0.0f, 0.0f, 0.0f }, 0);
    exploration world{ bohater };

    gamestate* active_state = &world;

    std::string player_name = "";
    bool niepoprawny_nick = false;

    int current_state = 1;

    

    graphics_init();
    LoadGameTextures();
    LoadGameModels();

    arial_font = LoadFontEx("Fonts/arial.ttf", 32, NULL, 0);
    pogrubione_arial_font = LoadFontEx("Fonts/arialbd.ttf", 32, NULL, 0);
    GuiSetFont(arial_font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

    battle* current_fight = nullptr;


    while (!WindowShouldClose())
    {
        
        global_fx.update();
        Vector3 stara_pozycja = world.camera.position;
        Vector3 stara_camera = world.camera.target;

        
        SetExitKey(KEY_NULL);

        //if (current_state == 0)
        //{
        //    int key = GetCharPressed();
        //    while (key > 0)
        //    {
        //        if ((key >= 32) && (key <= 125) && (player_name.length() < 12))
        //        {
        //            player_name += (char)key;
        //        }
        //        key = GetCharPressed();
        //    }

        //    if (IsKeyPressed(KEY_BACKSPACE) && !player_name.empty())
        //    {
        //        player_name.pop_back();
        //    }

        //    if (IsKeyPressed(KEY_ENTER))
        //    {
        //        if (bohater.validate_and_set_name(player_name))
        //        {
        //            current_state = 1;
        //            niepoprawny_nick = false;

        //        }
        //        else
        //        {
        //            niepoprawny_nick = true;
        //        }
        //    }

        //    //BeginDrawing();
        //    ////draw_login_screen(player_name, niepoprawny_nick);
        //    //EndDrawing();

        //    continue;
        //}
        
        if (current_state == 1)
        {
            UpdateCamera(&world.camera, CAMERA_FIRST_PERSON);
            world.apply_collision(stara_pozycja);
            world.event_check();
            
            
            for (auto& [id, node] : world.world_map)
            {
                enemy* current_enemy = node.enemy;
                if (current_enemy == nullptr || current_enemy->is_dead())
                    continue;

                float dystans = Vector3Distance(world.camera.position, current_enemy->get_position());
                if (dystans < 1.2f)
                {
                    current_state = 2;
                    current_fight = new battle(bohater, *current_enemy);
                    bohater.xp_from_enemy_dif = current_enemy->get_dif();
                    current_fight->exp = &world;
                    active_state = current_fight;
                }
            }
            
            
        }
        else
        {
            world.camera.projection = CAMERA_PERSPECTIVE;
        }

        if (bohater.is_dead()) current_state = 5;

        if (current_state == 5)
        {
            BeginDrawing();
            draw_game_over();
            EndDrawing();
            continue;

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
        else if (nextID == -1 && current_state == 4)
        {
            map_state* map = (map_state*)active_state;

            active_state = map->get_previous_state();
            current_state = (active_state == &world) ? 1 : 2;

            delete map;
        }
        if (nextID == 6 && current_state != 6)
        {
            current_state = 6;
        
        }
        else if (current_state == 6 && world.active_ui_event == nullptr)
        {
            current_state = 1;

        }

        static int prev_cursor_state = -1;
        if (current_state != prev_cursor_state)
        {
            if (current_state == 1)
            {
                DisableCursor();
            }
            else
            {
                 EnableCursor();
            }
               
            prev_cursor_state = current_state;
        }

        BeginDrawing();
        ClearBackground(BLACK);


        active_state->draw();
        if (current_state==2)
            draw_commentary();
        EndDrawing();

        if (current_state == 2)
        {
            if (current_fight != nullptr)
            {
                enemy* your_opponent = &current_fight->get_enemy();

                if (your_opponent && your_opponent->is_dead())
                {
                    bohater.grant_xp();
                    bohater.check_level_up();

                    chest* dropped_loot = world.rand_loot(your_opponent);
                    if (dropped_loot != nullptr)
                    {
                        world.world_chests.push_back(dropped_loot);
                    }

                    active_state = &world;
                    current_state = 1;

                    delete current_fight;
                    current_fight = nullptr;

                    world.delete_enemy(your_opponent);
                }

            }
        }
 
    }
    
    UnloadFont(arial_font);
    CloseWindow();
    
    return 0;
}
        
