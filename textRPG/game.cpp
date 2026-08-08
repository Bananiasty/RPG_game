#include <iostream>
#include <ctime>
#include <cstdlib>
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
    dlugosc = 200;
    szerokosc = 200;

    dungeon.assign(szerokosc, std::vector<int>(dlugosc, 2));

    world_map_init();
    generate_dungeon();

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

chest_drop::chest_drop(player& p, chest* chest) : Event(), p_ref(p), chest_ptr(chest){}
enemy_loot::enemy_loot(player& p, enemy* e, chest* chest) : Event(), p_ref(p), e_ref(e), chest_ptr(chest) {}





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
        enemy_loot* el = dynamic_cast<enemy_loot*>(active_ui_event);
        if (el != nullptr && !el->is_loot_open)
        {
            active_ui_event = nullptr;
            return;
        }

        chest_drop* cd = dynamic_cast<chest_drop*>(active_ui_event);
        if (cd != nullptr && !cd->is_chest_open)
        {
            active_ui_event = nullptr;
            return;
        }

        return; 
    }


    Vector3 player_pos = camera.position;

    for (chest* c : world_chests)
    {
        if (c == nullptr)
        {
            continue;
        }

        if (Vector3Distance(player_pos, c->position) <= 2.5f && IsKeyPressed(KEY_E))
        {
            if (c->enemy_ptr != nullptr)
            {
                if (!c->enemy_ptr->loot.empty() || !c->enemy_loot.empty())
                {
                    enemy_loot* new_loot = new enemy_loot(bohater, c->enemy_ptr, c);
                    new_loot->is_loot_open = true;
                    active_ui_event = new_loot;
                }
            }
            else
            {
                if (!c->chest_loot.empty())
                {
                    chest_drop* new_chest = new chest_drop(bohater, c);
                    new_chest->is_chest_open = true;
                    active_ui_event = new_chest;
                }
            }
            break;
        }
    }
}



Vector3 exploration::set_enemy_pos(int enemy_id, int room_id)
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
    Node* room_ptr = this->get_room_by_id(room_id);

    float enemy_y = enemy_ptr->get_position().y;

    Vector3 final_enemy_pos;
    float centerX = room_ptr->room_x + (room_ptr->room_width / 2.0f);
    float centerZ = room_ptr->room_y + (room_ptr->room_length / 2.0f);

    final_enemy_pos = { centerX * 2.0f, enemy_y, centerZ * 2.0f };

    return final_enemy_pos;
}



void chest_drop::draw_event(exploration* exp)
{
    this->exp = exp;
    if (draw_drop(exp, this->chest_ptr, this->is_chest_open))
    {
        this->discard_chest();
    }
}
void enemy_loot::draw_event(exploration* exp)
{
    this->exp = exp;
    if (draw_drop(exp, this->chest_ptr, this->is_loot_open))
    {
        this->discard_enemy_items();
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


