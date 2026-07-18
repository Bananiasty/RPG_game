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
    generate_map_from_graph();
   
    

    this->camera = { 0 };
    this->camera.position = Vector3{ 40.0f, 1.7f, 62.0f };
    this->camera.target = Vector3{ 40.0f, 1.7f, 68.0f };
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
        if (p_ref.spell_queued)
        {

            global_fx.texture = *(p_ref.queued_animation_texture);
            global_fx.frame_count = p_ref.queued_frame_count;
            global_fx.frame_time = p_ref.queued_frame_time;

            Vector2 target_pos = { 680, 260 };
            global_fx.play(target_pos);

            this->waiting_for_enemy = true;

            this->enemy_cooldown = (p_ref.queued_frame_count * p_ref.queued_frame_time) + 2;

            p_ref.spell_queued = false;
        }
        else if (this->attack_clicked && click_cooldown == false)
        {
            this->attack_clicked = false;
            this->player_attack();
            this->waiting_for_enemy = true;
            this->click_cooldown = true;

            this->enemy_cooldown = 2.0;
        }

            
    }
 
    if (click_cooldown)
    {
        player_cooldown -= GetFrameTime();
        if (player_cooldown <= 0.0)
        {
            click_cooldown = false;
        }
    }

    if (waiting_for_enemy) {
        if (!global_fx.is_playing && p_ref.queued_damage > 0)
        {
            e_ref.take_damage(p_ref.queued_damage, &p_ref, false);
            p_ref.queued_damage = 0.0;
        }

        enemy_cooldown -= GetFrameTime();       
        if (enemy_cooldown <= 0.0) {
            this->enemy_turn();
            waiting_for_enemy = false;
        }

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
        return;
    }

    Vector3 player_pos = camera.position;

    for (chest* c : world_chests)
    {
        if (c != nullptr)
        {
            float distance = Vector3Distance(player_pos, c->position);

            if (distance <= 2.5f && IsKeyPressed(KEY_E))
            {
                if (c->enemy_ptr != nullptr)
                {
                    enemy_loot* new_loot = new enemy_loot(bohater, c->enemy_ptr, c);
                    new_loot->is_loot_open = true;
                    active_ui_event = new_loot;
                }
                else
                {
                    chest_drop* new_chest = new chest_drop(bohater, c);
                    new_chest->is_chest_open = true;
                    active_ui_event = new_chest;
                }
                break;
            }
        }
    }
}


void exploration::move_left()
{
    if (world_map[current_node_id].left_id != -1)
    {
        current_node_id = world_map[current_node_id].left_id;

        Node* current_Node = get_current_node();

        if (world_map[current_node_id].left_id != -1)
        {
            world_map[current_Node->left_id].discovered = true;
            world_map[current_Node->right_id].discovered = true;
        }

        if (world_map[current_node_id].right_id != -1)
        {   
            world_map[current_Node->left_id].discovered = true;
            world_map[current_Node->right_id].discovered = true;
        }
        event_check();
    }
}

void exploration::move_right()
{
    if (world_map[current_node_id].right_id != -1)
    {
        current_node_id = world_map[current_node_id].right_id;

        Node* current_Node = get_current_node();

        if (world_map[current_node_id].left_id != -1)
        {
            world_map[current_Node->left_id].discovered = true;
            world_map[current_Node->right_id].discovered = true;
        }

        if (world_map[current_node_id].right_id != -1)
        {
            world_map[current_Node->left_id].discovered = true;
            world_map[current_Node->right_id].discovered = true;
        }
        event_check();
    }
}

void exploration::move_back()
{
    if (world_map[current_node_id].previous_id != -1)
    {
        current_node_id = world_map[current_node_id].previous_id;
    }
    event_check();
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

    Node* current = get_current_node();

    if (active_ui_event != nullptr)
    {
        active_ui_event->draw_event(this);
    }


}
void battle::draw() 
{
    draw_battle_ui(this);
    DrawGlobalAnimation();
    
}
void inventory_state::draw() 
{
    
    if (get_previous_state() == exp)
    {
        draw_game_scene(exp);
    }
    else
    {
        draw_battle_ui(fight);
    }
    draw_inventory_ui(p_ref, this);
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
    draw_map(this, exp);
}


