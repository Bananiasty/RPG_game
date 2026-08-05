#include <iostream>
#include <ctime>
#include <cstdlib>
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
	exploration* exp = this->exp;
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

	Vector3 look_at_player = Vector3Subtract(p_pos, target_e_pos);
	look_at_player.y = 0.0f;
	look_at_player = Vector3Normalize(look_at_player);

	e.set_forward(look_at_player);
	e.set_position(target_e_pos);

	Vector3 cam_target = target_e_pos;

	exp->camera.target = cam_target;

}

int battle::player_turn()
{
    auto params = GetGhoulRenderParams(e_ref.get_position(), textures.ghoul);

    BodyPart hovered = exp->GetHoveredBodyPart(params.drawPos, params.targetWidth, params.targetHeight, exp->camera);
    e_ref.set_hovered_body_part(hovered);

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
                auto [p_dmg, crit] = p_ref.calculate_dmg();
                int final_dmg = e_ref.take_damage(p_dmg, &p_ref, crit, p_ref.is_guard);

                this->waiting_for_enemy = true;
                this->click_cooldown = true;
                this->enemy_cooldown = 1.5;

                this->attack_clicked = false;
                e_ref.set_hovered_body_part(BodyPart::NONE);

                return final_dmg;
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

                Vector2 target_pos = { 680, 260 };
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
		if (!global_fx.is_playing && p_ref.queued_damage > 0)
		{
			e_ref.take_damage(p_ref.queued_damage, &p_ref, false, false);
			p_ref.queued_damage = 0.0;
		}
		auto [e_dmg, crit] = e_ref.calculate_dmg();
		int final_dmg = p_ref.take_damage(e_dmg, &p_ref, crit, p_ref.is_guard);

		p_ref.is_guard = false;

		return final_dmg;
	}
	return 0;
}
