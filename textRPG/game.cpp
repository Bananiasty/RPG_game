#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "graphics.h"
#include "raylib.h"
#include "textureManager.h"
#include "raymath.h"


std::vector<std::string> gamestate::gameLogs;

battle::battle(player& p, enemy& e) : p_ref(p), e_ref(e) 
{ 
    gamestate::gameLogs.clear();
    p_ref.current_enemy = &e_ref;
    log_object_intro(e_ref);
    
}

exploration::exploration(player& p): bohater(p)
{
    active_ui_event = nullptr;
    current_node_id = 1;
    current_floor_id = 0;       
    const int total_floors = 2;
    floors.resize(total_floors);
    dlugosc = 200;
    szerokosc = 200;

    for (auto& floor : floors)
    {
        floor.dungeon.assign(szerokosc, std::vector<int>(dlugosc, 2));
    }

    world_map_init();
    generate_floor(current_floor_id);

    player_stats_init(&bohater);
    

    this->camera = { 0 };
    this->camera.position = Vector3{ bohater.position.x, 1.5f, bohater.position.z };
    this->camera.target = Vector3{ bohater.position.x, 1.5f, bohater.position.z + 5.0f };
    this->camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    this->camera.fovy = 80.0f;
    this->camera.projection = CAMERA_PERSPECTIVE;


}

inventory_state::inventory_state(player& p, gamestate* back_to) : p_ref(p), previous_state(back_to){}

map_state::map_state(gamestate* back_to) :previous_state(back_to){}



int exploration::update_state()
 {
    this->bohater.position.x = this->camera.position.x;
    this->bohater.position.z = this->camera.position.z;
    if (IsKeyPressed(KEY_I))
    {
        showInventory = !showInventory;
        return 3;
    }
    if (IsKeyPressed(KEY_M))
    {
        showMap = !showMap;
        return 4;
    }
    update_enemies();
    event_check();
    if (active_ui_event != nullptr)
    {
        return 6;
    }

    return 1;
}


int battle::update_state()
{
    if (e_ref.is_dead())
    {
        return 1; 
    }
    if (p_ref.is_dead())
    {
        return 5; 
    }

    
    if (IsKeyPressed(KEY_I) || this->showInventory)
    {
        this->showInventory = false;
        return 3;
    }
    if (IsKeyPressed(KEY_M))
    {
        return 4;
    }

    if (!waiting_for_enemy)
    {
        this->player_turn();           
    }
   
    if (waiting_for_enemy) 
    {      
        this->enemy_turn();
    }

    return 2; 
}


int inventory_state::update_state()
{
    if (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_ESCAPE))
    {     
        
        return -1;
    }
    return 3;

}

int map_state::update_state()
{
    if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE))
    {
        return -1;
    }
    return 4;
    
}

void exploration::event_check()
{
    if (active_ui_event != nullptr)
    {
        loot_event* current_loot = dynamic_cast<loot_event*>(active_ui_event);
        if (current_loot != nullptr && !current_loot->is_active)
        {
            delete active_ui_event;
            active_ui_event = nullptr;
        }
        return;
    }


    Vector3 player_pos = camera.position;

    auto& current_loot_list = floors[current_floor_id].world_loot;

    for (drop_object* loot_obj : current_loot_list)
    {
        if (loot_obj == nullptr)
        {
            continue;
        }

        if (Vector3Distance(player_pos, loot_obj->position) <= 2.5f && IsKeyPressed(KEY_E))
        {
            if (!loot_obj->drop_loot.empty())
            {
                active_ui_event = new loot_event(this, bohater, loot_obj);
            }
            break;
        }
    }
}



Vector3 exploration::set_enemy_pos(int enemy_id, int room_id, int floor_id)
{
    if (enemy_id == -1)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    enemy* enemy_ptr = this->get_enemy_by_id(enemy_id);
    if (enemy_ptr == nullptr)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    Node* room_ptr = this->get_room_by_id(room_id, floor_id);
    if (room_ptr == nullptr)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    float enemy_y = enemy_ptr->get_position().y;
    float centerX = room_ptr->room_x + (room_ptr->room_width / 2.0f);
    float centerZ = room_ptr->room_y + (room_ptr->room_length / 2.0f);

    return Vector3{ centerX * 2.0f, enemy_y, centerZ * 2.0f };
}

void gamestate::spawn_floating_text(Vector3 pos, const std::string& text, bool is_crit)
{
    floating_text ft;
    ft.hit_limb = pos;
    ft.text = text;
    ft.is_crit = is_crit;
    ft.max_lifetime = 1.0f;
    ft.offsetY = 0.0f;

    this->active_texts.push_back(ft);
}

void gamestate::update_and_draw_floating_texts(Camera3D current_camera)
{
    float deltaTime = GetFrameTime();

    for (auto& dt : this->active_texts)
    {
        dt.max_lifetime -= deltaTime;
        dt.offsetY += 35.0f * deltaTime;

        Vector2 screenPos = GetWorldToVirtualScreen(dt.hit_limb, current_camera, (float)GAME_WIDTH, (float)GAME_HEIGHT);
        screenPos.y -= dt.offsetY;

        float alpha = std::clamp(dt.max_lifetime, 0.0f, 1.0f);

        Color baseColor = dt.is_crit ? RED : WHITE;
        Color textColor = Fade(baseColor, alpha);
        Color shadowColor = Fade(BLACK, alpha);

        int fontSize = dt.is_crit ? 60 : 50;

        int textWidth = MeasureText(dt.text.c_str(), fontSize);
        int drawX = static_cast<int>(screenPos.x - textWidth / 2.0f);
        int drawY = static_cast<int>(screenPos.y);

        DrawText(dt.text.c_str(), drawX + 2, drawY + 2, fontSize, shadowColor);
        DrawText(dt.text.c_str(), drawX, drawY, fontSize, textColor);
    }

    std::erase_if(this->active_texts, [](const floating_text& dt) {
        return dt.max_lifetime <= 0.0f;
        });
}


void loot_event::draw_event(exploration* exp)
{
    this->exp = exp;
    if (draw_drop(exp, this->target_container, this->is_active))
    {
        this->is_active = false;
    }
}

void exploration::draw() 
{
    draw_game_scene(this);
    draw_buttons(this);
    draw_menu();

    if (active_ui_event != nullptr)
    {
        active_ui_event->draw_event(this);
    }


}
void battle::draw() 
{
    draw_game_scene(this->exp);
    draw_battle_ui(this);
    draw_buttons(this->exp);
    DrawGlobalAnimation();
    
}
void inventory_state::draw() 
{
    
    draw_game_scene(exp);
    if (get_previous_state() != exp)
    {
        draw_battle_ui(fight);
    }
    draw_inventory_ui(p_ref, this);
    draw_buttons(this->exp);
}
void map_state::draw()
{
    if (get_previous_state() == exp)
    {
        draw_game_scene(exp);
    }
    else
    {
        draw_battle_ui(fight);
    }
    draw_dungeon_map(this->exp, this->exp->bohater.position.x, this->exp->bohater.position.z);
    draw_buttons(this->exp);
}


