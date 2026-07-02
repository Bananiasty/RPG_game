#include <iostream>
#include <ctime>
#include <cstdlib>
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "graphics.h"
#include "raygui.h"
#include "textureManager.h"

std::vector<std::string> gamestate::gameLogs;

battle::battle(player& p, enemy& e) : p_ref(p), e_ref(e) 
{ 
    gamestate::gameLogs.clear();
    p_ref.current_enemy = &e_ref;

    log_object_intro(e_ref);
    can_save_game = false;
    
}

exploration::exploration(player& p): bohater(p)
{
    can_save_game = true;
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

inventory_state::inventory_state(player& p, gamestate* back_to) : p_ref(p), previous_state(back_to), back_requested(false) 
{
    can_save_game = false;
}

map_state::map_state(gamestate* back_to) :previous_state(back_to), back_requested(false) 
{
    can_save_game = false;
}

chest_drop::chest_drop(player& p, chest* chest) : Event(), chest_ptr(chest)
{ 
    can_save_game = true; 
}








int exploration::update_state()
 {
    if (this->showInventory) {
        this->showInventory = false;
        return 3;
    }
    if (this->showMap)
    {
        this->showMap = false;
        return 4;
    }
    if (world_map[current_node_id].enemy != nullptr)
    {
        return 2;
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

    if (this->showInventory) {
        this->showInventory = false;
        return 3;
    }
    if (this->showMap)
    {
        this->showMap = false;
        return 4;
    }

    if (!waiting_for_enemy)
    {
        if (p_ref.spell_queued)
        {
            std::cout << "[DEBUG] Rzucam czar!" << std::endl;
            std::cout << "[DEBUG] Klatki: " << p_ref.queued_frame_count << " | Czas klatki: " << p_ref.queued_frame_time << std::endl;
            std::cout << "[DEBUG] Obrazenia: " << p_ref.queued_damage << std::endl;

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
    if(this->back_requested)
    {
        return -1;
    }
    return 3;
}

int map_state::update_state()
{
    return 4;
}

void exploration::event_check()
{
    Node* current = get_current_node();
    if (current->spawn_chest != nullptr && current->current_event == nullptr)
    {
        current->current_event = new chest_drop(bohater, current->spawn_chest);
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
    draw_chest_drop(exp);
}

void exploration::draw() 
{
    draw_game_scene(this);
    //draw_buttons(this);
    //draw_menu();

    /*Node* current = get_current_node();

    if (current->current_event != nullptr && get_current_enemy() == nullptr)
    {
        current->current_event->draw_event(this);
    }*/


}
void battle::draw() 
{
    draw_battle_ui(this);
    DrawGlobalAnimation();
    draw_menu();
}
void inventory_state::draw() 
{
    draw_game_scene(exp);
    if (get_previous_state() != exp)
        draw_battle_ui(fight);
    draw_inventory_ui(p_ref, this);
    draw_menu();
}
void map_state::draw()
{
    draw_game_scene(exp);
    if (get_previous_state() != exp)
        draw_battle_ui(fight);
    draw_map(this, exp);
    draw_menu();
}


