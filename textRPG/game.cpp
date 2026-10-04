#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <memory>

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

inventory_state::inventory_state(player& p, exploration& w, gamestate* back_to) : p_ref(p), world(w), previous_state(back_to){}

map_state::map_state(gamestate* back_to) :previous_state(back_to){}



int exploration::update_state()
 {
    float dt = GetFrameTime();

    for (auto& obj : floors[current_floor_id].world_objects)
    {
        if (obj != nullptr)
        {
            obj->update(dt);
        }
    }
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
        if (!this->pending_drops.empty())
        {
            // Spawnowanie sakiewki na pozycji gracza
            world.spawn_loot_bag(p_ref.get_position(), this->pending_drops);

            // Czyścimy bufor sesji
            this->pending_drops.clear();
        }
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



void gamestate::spawn_floating_text(Vector3 pos, const std::string& text, bool crit, bool bleed)
{
    floating_text ft;
    ft.hit_limb = pos;
    ft.text = text;
    ft.is_crit = crit;
    ft.is_bleed = bleed;
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

        Color baseColor = WHITE;
        if (dt.is_bleed)
        {
            baseColor = RED;
        }
        else if (dt.is_crit)
        {
            baseColor = PURPLE;
        }

        Color textColor = Fade(baseColor, alpha);
        Color shadowColor = Fade(BLACK, alpha);

        int fontSize = dt.is_crit ? 60 : (dt.is_bleed ? 45 : 50);

        int textWidth = MeasureText(dt.text.c_str(), fontSize);
        int drawX = static_cast<int>(screenPos.x - textWidth / 2.0f);
        int drawY = static_cast<int>(screenPos.y);

        DrawText(dt.text.c_str(), drawX + 2, drawY + 2, fontSize, shadowColor);
        DrawText(dt.text.c_str(), drawX, drawY, fontSize, textColor);
    }
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
}


