#include <iostream>
#include <ctime>
#include <cstdlib>
#include "graphics.h"
#include "character.h"
#include "inventory_class.h"
#include "gamestates.h"
#include "struct.h"
#include "raylib.h"
#include "raymath.h"
#include "TextureManager.h"



void battle::initiate_fight_view()
{
    enemy& e = this->e_ref;
    player& p = this->p_ref;
    e.exp = this->exp;

    Vector3 e_pos = e.get_position();
    Vector3 p_pos = p.get_position();

    Vector3 dir = Vector3Subtract(e_pos, p_pos);
    dir.y = 0.0f;

    if (Vector3Length(dir) == 0.0f)
    {
        dir = { 0.0f, 0.0f, 1.0f };
    }
    dir = Vector3Normalize(dir);
    float desired_distance = 2.5f;

    Vector3 target_e_pos = Vector3Add(p_pos, Vector3Scale(dir, desired_distance));
    target_e_pos.y = e_pos.y;

    e.set_position(target_e_pos);

    exp->camera.target = target_e_pos;

    Vector3 dirToCamera = Vector3Subtract(exp->camera.position, target_e_pos);
    dirToCamera.y = 0.0f;

    if (Vector3Length(dirToCamera) > 0.0f)
    {
        e.set_forward(Vector3Normalize(dirToCamera));
    }
}

int battle::player_turn()
{
    auto params = e_ref.get_render_params(textures.ghoul);

    if (!p_ref.is_dead() && !e_ref.is_dead())
    {
        if (click_cooldown)
        {
            player_cooldown -= GetFrameTime();
            if (player_cooldown <= 0.0)
            {
                click_cooldown = false;
            }
        }
        else
        {
            if (this->attack_clicked)
            {
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    this->attack_clicked = false;
                    e_ref.set_hovered_body_part(BodyPart::NONE);
                    return 0;
                }

                BodyPart hovered = e_ref.calculate_hovered_body_part(params.drawPos, params.targetWidth, params.targetHeight, exp->camera);
                e_ref.set_hovered_body_part(hovered);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered != BodyPart::NONE)
                {
                    Vector3 forward = Vector3Normalize(Vector3Subtract(exp->camera.target, exp->camera.position));
                    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, exp->camera.up));
                    Vector3 up = Vector3Normalize(exp->camera.up);

                    float halfW = params.targetWidth * 0.5f;
                    float halfH = params.targetHeight * 0.5f;
                    Vector3 center = params.drawPos;

                    Vector3 topLeft = Vector3Add(Vector3Subtract(center, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
                    Vector3 topRight = Vector3Add(Vector3Add(center, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
                    Vector3 bottomRight = Vector3Subtract(Vector3Add(center, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
                    Vector3 bottomLeft = Vector3Subtract(Vector3Subtract(center, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));

                    Ray ray = GetScreenToWorldRay(GetMousePosition(), exp->camera);
                    RayCollision collision = GetRayCollisionQuad(ray, topLeft, topRight, bottomRight, bottomLeft);

                    Vector3 hit_point = collision.hit ? collision.point : center;

                    auto [p_dmg, crit] = p_ref.calculate_dmg();

                    int final_dmg = e_ref.take_damage(p_dmg, &p_ref, crit, p_ref.is_guard, hovered, this->exp, hit_point);

                    this->waiting_for_enemy = true;
                    this->click_cooldown = true;
                    this->enemy_cooldown = 1.5;

                    this->attack_clicked = false;
                    e_ref.set_hovered_body_part(BodyPart::NONE);

                    return final_dmg;
                }
            }
            if (this->guard_clicked)
            {
                this->p_ref.player_guard();

                this->waiting_for_enemy = true;
                this->click_cooldown = true;
                this->enemy_cooldown = 1.5;

                this->guard_clicked = false;
                e_ref.set_hovered_body_part(BodyPart::NONE);

                return 0;
            }
            if (p_ref.spell_queued)
            {
                global_fx.texture = *(p_ref.queued_animation_texture);
                global_fx.frame_count = p_ref.queued_frame_count;
                global_fx.frame_time = p_ref.queued_frame_time;

                Vector2 target_pos = { GAME_WIDTH / 2.0f, GAME_HEIGHT / 2.0f };
                global_fx.play(target_pos);

                this->waiting_for_enemy = true;
                this->enemy_cooldown = (p_ref.queued_frame_count * p_ref.queued_frame_time) + 2;

                p_ref.spell_queued = false;
                e_ref.set_hovered_body_part(BodyPart::NONE);
            }
        }
    }

    return 0;
}


int battle::enemy_turn()
{
    enemy_cooldown -= GetFrameTime();
    if (enemy_cooldown <= 0.0)
    {
        waiting_for_enemy = false;
        return e_ref.execute_ai_turn(p_ref);
    }

    return 0;
}
