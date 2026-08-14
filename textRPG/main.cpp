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
#include "AudioManager.h"

Font cabin_sketch_font;
Font cabin_sketch_font_bold;

int main()
{
    setlocale(LC_CTYPE, "Polish");
    srand(time(NULL));

    player bohater("", {20, 60, 30, 30, 30, 30}, 0, 10, 30, 30, 30, 0, 0, 1, textures.player, {0.0f, 0.0f, 0.0f}, 0);
    exploration world{ bohater };

    gamestate* active_state = &world;

    std::string player_name = "";
    bool niepoprawny_nick = false;

    int current_state = 1;
    float moveSpeed;
    float mouseSensitivity;
    
    graphics_init();
    LoadGameTextures();
    LoadGameModels();
    LoadGameAudio();
    audio.play_music(MusicID::EXPLORATION);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    cabin_sketch_font = LoadFontEx("Fonts/Cabin sketch/CabinSketch-Regular.ttf", 36, NULL, 0);
    cabin_sketch_font_bold = LoadFontEx("Fonts/Cabin sketch/CabinSketch-Bold.ttf", 36, NULL, 0);

    GuiSetFont(cabin_sketch_font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

    battle* current_fight = nullptr;


    while (!WindowShouldClose())
    {
        audio.update();
        float scale = fminf((float)GetScreenWidth() / GAME_WIDTH, (float)GetScreenHeight() / GAME_HEIGHT);
        Vector2 real_mouse = GetMousePosition();

        Vector2 virtual_mouse = {
            (real_mouse.x - (GetScreenWidth() - (GAME_WIDTH * scale)) * 0.5f) / scale,
            (real_mouse.y - (GetScreenHeight() - (GAME_HEIGHT * scale)) * 0.5f) / scale
        };
        virtual_mouse_pos = virtual_mouse;

        global_fx.update();
        Vector3 stara_pozycja = world.camera.position;
        Vector3 stara_camera = world.camera.target;

        SetExitKey(KEY_NULL);

        if (current_state == 1 && !world.showMenu)
        {
            moveSpeed = 0.1f;
            mouseSensitivity = 0.1f;
            Vector3 movement = {
                ((float)IsKeyDown(KEY_W) - (float)IsKeyDown(KEY_S)) * moveSpeed,
                ((float)IsKeyDown(KEY_D) - (float)IsKeyDown(KEY_A)) * moveSpeed,
                0.0f
            };

            Vector3 rotation = {
                GetMouseDelta().x * mouseSensitivity,
                GetMouseDelta().y * mouseSensitivity,
                0.0f
            };
            UpdateCameraPro(&world.camera, movement, rotation, 0.0f);
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
                    current_fight->initiate_fight_view();
                    active_state = current_fight;
                }
            }
        }
        else
        {
            world.camera.projection = CAMERA_PERSPECTIVE;
        }

        if (bohater.is_dead()) current_state = 5;

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
        static bool prev_show_menu = false;

        if (current_state != prev_cursor_state || world.showMenu != prev_show_menu)
        {
            if (current_state == 1 && !world.showMenu)
            {
                DisableCursor();
            }
            else
            {
                EnableCursor();
            }

            prev_cursor_state = current_state;
            prev_show_menu = world.showMenu;
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (current_state == 5)
        {
            draw_game_over();
        }
        else
        {
            active_state->draw();
            if (current_state == 2)
            {
                draw_commentary();
            }
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        Rectangle source_rec = { 0.0f, 0.0f, (float)GAME_WIDTH, (float)-GAME_HEIGHT };
        Rectangle dest_rec = {
            (GetScreenWidth() - ((float)GAME_WIDTH * scale)) * 0.5f,
            (GetScreenHeight() - ((float)GAME_HEIGHT * scale)) * 0.5f,
            (float)GAME_WIDTH * scale,
            (float)GAME_HEIGHT * scale
        };

        DrawTexturePro(target.texture, source_rec, dest_rec, { 0.0f, 0.0f }, 0.0f, WHITE);
        EndDrawing();

        if (current_state == 2 && current_fight != nullptr)
        {
            enemy* your_opponent = &current_fight->get_enemy();
            if (your_opponent != nullptr)
            {
                your_opponent->set_last_frame(0);
                if (your_opponent->is_dead())
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

    UnloadRenderTexture(target);
    UnloadFont(cabin_sketch_font);
    UnloadFont(cabin_sketch_font_bold);
    CloseWindow();

    return 0;
}
        
