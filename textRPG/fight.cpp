#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "graphics.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "gamestates.h"
#include "struct.h"
#include "raylib.h"
#include "raymath.h"
#include "TextureManager.h"
#include "AudioManager.h"



void battle::initiate_fight_view()
{
    enemy& e = this->e_ref;
    player& p = this->p_ref;
    e.exp = this->exp;
    auto& current_dungeon = exp->floors[exp->current_floor_id].dungeon;

    Vector3 e_pos = e.get_position();
    Vector3 p_pos = p.get_position();

    const float DUNGEON_TILE = 2.0f;

    Vector3 raw_diff = Vector3Subtract(e_pos, p_pos);
    raw_diff.y = 0.0f;

    Vector3 primary_dir = { 0.0f, 0.0f, 0.0f };
    if (fabsf(raw_diff.x) >= fabsf(raw_diff.z))
    {
        primary_dir.x = (raw_diff.x >= 0.0f) ? 1.0f : -1.0f;
    }
    else
    {
        primary_dir.z = (raw_diff.z >= 0.0f) ? 1.0f : -1.0f;
    }

    float p_center_x = floorf(p_pos.x / DUNGEON_TILE) * DUNGEON_TILE + (DUNGEON_TILE * 0.5f);
    float p_center_z = floorf(p_pos.z / DUNGEON_TILE) * DUNGEON_TILE + (DUNGEON_TILE * 0.5f);
    Vector3 target_p_pos = { p_center_x, p_pos.y, p_center_z };

    bool player_in_room = false;
    if (exp && !exp->floors.empty() && exp->current_floor_id >= 0 && exp->current_floor_id < static_cast<int>(exp->floors.size()))
    {
        const auto& cur_floor = exp->floors[exp->current_floor_id];
        int p_gx = static_cast<int>(floorf(target_p_pos.x / DUNGEON_TILE)) + 1;
        int p_gz = static_cast<int>(floorf(target_p_pos.z / DUNGEON_TILE));
        for (const auto& [id, node] : cur_floor.world_map)
        {
            if (p_gx >= node.room_x && p_gx < node.room_x + node.room_width &&
                p_gz >= node.room_y && p_gz < node.room_y + node.room_length)
            {
                player_in_room = true;
                break;
            }
        }
    }

    Vector3 test_pos = Vector3Add(target_p_pos, Vector3Scale(primary_dir, DUNGEON_TILE));
    bool primary_blocked = !is_tile_walkable_at(test_pos);

    Vector3 test_side = Vector3Normalize(Vector3CrossProduct(primary_dir, { 0.0f, 1.0f, 0.0f }));
    bool left_walk = is_tile_walkable_at(Vector3Subtract(test_pos, Vector3Scale(test_side, DUNGEON_TILE)));
    bool right_walk = is_tile_walkable_at(Vector3Add(test_pos, Vector3Scale(test_side, DUNGEON_TILE)));
    bool facing_narrow = (!left_walk && !right_walk);

    Vector3 final_dir = primary_dir;

    if (primary_blocked || (player_in_room && facing_narrow))
    {
        const Vector3 CARDINAL_DIRS[4] = {
            {  1.0f, 0.0f,  0.0f },
            { -1.0f, 0.0f,  0.0f },
            {  0.0f, 0.0f,  1.0f },
            {  0.0f, 0.0f, -1.0f }
        };

        Vector3 norm_raw = Vector3Normalize(raw_diff);
        float best_score = -9999.0f;

        for (int i = 0; i < 4; i++)
        {
            Vector3 cand_dir = CARDINAL_DIRS[i];
            Vector3 cand_e_pos = Vector3Add(target_p_pos, Vector3Scale(cand_dir, DUNGEON_TILE));

            if (!is_tile_walkable_at(cand_e_pos))
            {
                continue;
            }

            Vector3 cand_side = Vector3Normalize(Vector3CrossProduct(cand_dir, { 0.0f, 1.0f, 0.0f }));
            int left_c = 0;
            for (int step = 1; step <= 3; ++step)
            {
                Vector3 p_step = Vector3Subtract(cand_e_pos, Vector3Scale(cand_side, DUNGEON_TILE * step));
                if (is_tile_walkable_at(p_step)) left_c++;
                else break;
            }
            int right_c = 0;
            for (int step = 1; step <= 3; ++step)
            {
                Vector3 p_step = Vector3Add(cand_e_pos, Vector3Scale(cand_side, DUNGEON_TILE * step));
                if (is_tile_walkable_at(p_step)) right_c++;
                else break;
            }

            int capacity = 1 + left_c + right_c;
            float score = 0.0f;

            if (player_in_room && facing_narrow)
            {
                score = capacity * 100.0f;
            }
            else
            {
                score = Vector3DotProduct(norm_raw, cand_dir) * 10.0f + capacity;
            }

            if (score > best_score)
            {
                best_score = score;
                final_dir = cand_dir;
            }
        }
    }

    Vector3 target_e_pos = Vector3Add(target_p_pos, Vector3Scale(final_dir, DUNGEON_TILE));
    target_e_pos.y = e_pos.y;

    p.set_position(target_p_pos);
    e.set_position(target_e_pos);

    exp->camera.position.x = target_p_pos.x;
    exp->camera.position.z = target_p_pos.z;
    exp->camera.target = target_e_pos;

    Vector3 dirToCamera = Vector3Subtract(exp->camera.position, target_e_pos);
    dirToCamera.y = 0.0f;

    if (Vector3Length(dirToCamera) > 0.0f)
    {
        e.set_forward(Vector3Normalize(dirToCamera));
    }
    e.set_last_frame(0);

    combat_facing_dir = final_dir;
    combat_enemy_center = target_e_pos;
    combat_player_pos = target_p_pos;

    active_enemies.clear();
    queued_enemies.clear();
    incoming_reinforcements.clear();
    pending_enemy_actions.clear();
    turn_start_effects_processed = false;
    active_enemies.push_back(&e);

    scan_reinforcements();
}

void battle::scan_reinforcements()
{
    if (exp == nullptr || exp->floors.empty() || exp->current_floor_id < 0 || exp->current_floor_id >= static_cast<int>(exp->floors.size()))
    {
        return;
    }

    auto& floor_enemies = exp->floors[exp->current_floor_id].active_enemies;
    float nodeSize = 0.5f;

    int p_grid_x = static_cast<int>(std::floor(combat_player_pos.x / nodeSize)) + 4;
    int p_grid_z = static_cast<int>(std::floor(combat_player_pos.z / nodeSize));

    for (enemy* other_e : floor_enemies)
    {
        if (other_e == nullptr || other_e->is_dead() || other_e == &e_ref)
        {
            continue;
        }

        // An enemy can join the battle as reinforcements only if already alerted when battle begins
        if (!other_e->get_is_alerted())
        {
            continue;
        }

        Vector3 other_pos = other_e->get_position();
        int e_grid_x = static_cast<int>(std::floor(other_pos.x / nodeSize)) + 4;
        int e_grid_z = static_cast<int>(std::floor(other_pos.z / nodeSize));

        std::vector<GridPosition> path = exp->find_path(e_grid_x, e_grid_z, p_grid_x, p_grid_z, other_e);

        if (!path.empty())
        {
            ReinforcementEnemy rein;
            rein.enemy_ptr = other_e;
            rein.path = path;
            rein.current_path_step = 0;
            rein.steps_per_round = (other_e->get_name() == "Pimpek") ? 8 : 4;

            int calculated_rounds = static_cast<int>(std::ceil(static_cast<float>(path.size()) / static_cast<float>(rein.steps_per_round)));
            rein.rounds_to_arrive = std::max(1, calculated_rounds);

            Vector3 player_right = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));
            Vector3 to_e = Vector3Subtract(other_pos, combat_player_pos);
            to_e.y = 0.0f;
            rein.is_approaching_from_left = (Vector3DotProduct(to_e, player_right) < 0.0f);

            incoming_reinforcements.push_back(rein);
        }
    }
}

bool battle::is_tile_walkable_at(Vector3 world_pos) const
{
    if (exp == nullptr) return false;
    const float DUNGEON_TILE = 2.0f;
    int gx = static_cast<int>(std::floor(world_pos.x / DUNGEON_TILE)) + 1;
    int gz = static_cast<int>(std::floor(world_pos.z / DUNGEON_TILE));
    return exp->is_walkable(gx, gz);
}

bool battle::is_player_in_room() const
{
    if (exp == nullptr || exp->floors.empty() || exp->current_floor_id < 0 || exp->current_floor_id >= static_cast<int>(exp->floors.size()))
    {
        return false;
    }

    const auto& cur_floor = exp->floors[exp->current_floor_id];
    const float DUNGEON_TILE = 2.0f;
    int gx = static_cast<int>(floorf(combat_player_pos.x / DUNGEON_TILE)) + 1;
    int gz = static_cast<int>(floorf(combat_player_pos.z / DUNGEON_TILE));

    for (const auto& [id, node] : cur_floor.world_map)
    {
        if (gx >= node.room_x && gx < node.room_x + node.room_width &&
            gz >= node.room_y && gz < node.room_y + node.room_length)
        {
            return true;
        }
    }

    return false;
}

bool battle::is_player_in_narrow_corridor() const
{
    if (exp == nullptr) return false;

    // If player is inside a room, they are not in a narrow corridor
    if (is_player_in_room())
    {
        return false;
    }

    // Player is in a corridor! Check if corridor width perpendicular to facing direction is 1 tile
    const float DUNGEON_TILE = 2.0f;
    Vector3 side_dir = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));

    bool left_blocked = !is_tile_walkable_at(Vector3Subtract(combat_player_pos, Vector3Scale(side_dir, DUNGEON_TILE)));
    bool right_blocked = !is_tile_walkable_at(Vector3Add(combat_player_pos, Vector3Scale(side_dir, DUNGEON_TILE)));

    return (left_blocked && right_blocked);
}

int battle::get_max_combat_capacity() const
{
    if (exp == nullptr) return 1;

    // Tactical positioning: If the PLAYER is in a narrow corridor, enemies must queue 1-by-1!
    if (is_player_in_narrow_corridor())
    {
        return 1;
    }

    const float DUNGEON_TILE = 2.0f;
    Vector3 side_dir = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));

    int left_tiles = 0;
    for (int step = 1; step <= 3; ++step)
    {
        Vector3 test_pos = Vector3Subtract(combat_enemy_center, Vector3Scale(side_dir, DUNGEON_TILE * step));
        if (is_tile_walkable_at(test_pos))
        {
            left_tiles++;
        }
        else
        {
            break;
        }
    }

    int right_tiles = 0;
    for (int step = 1; step <= 3; ++step)
    {
        Vector3 test_pos = Vector3Add(combat_enemy_center, Vector3Scale(side_dir, DUNGEON_TILE * step));
        if (is_tile_walkable_at(test_pos))
        {
            right_tiles++;
        }
        else
        {
            break;
        }
    }

    int total_tiles = 1 + left_tiles + right_tiles;
    return std::clamp(total_tiles, 1, 4);
}

bool battle::is_narrow_corridor_at_fight() const
{
    return get_max_combat_capacity() <= 1;
}

void battle::reposition_active_enemies()
{
    if (active_enemies.empty()) return;

    int n = static_cast<int>(active_enemies.size());
    if (n == 1)
    {
        active_enemies[0]->set_position(combat_enemy_center);
        Vector3 to_cam = Vector3Normalize(Vector3Subtract(combat_player_pos, combat_enemy_center));
        to_cam.y = 0.0f;
        active_enemies[0]->set_forward(to_cam);
        active_enemies[0]->set_last_frame(0);
        return;
    }

    Vector3 side_dir = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));
    const float DUNGEON_TILE = 2.0f;

    int left_tiles = 0;
    for (int step = 1; step <= 3; ++step)
    {
        Vector3 test_pos = Vector3Subtract(combat_enemy_center, Vector3Scale(side_dir, DUNGEON_TILE * step));
        if (is_tile_walkable_at(test_pos)) left_tiles++;
        else break;
    }

    int right_tiles = 0;
    for (int step = 1; step <= 3; ++step)
    {
        Vector3 test_pos = Vector3Add(combat_enemy_center, Vector3Scale(side_dir, DUNGEON_TILE * step));
        if (is_tile_walkable_at(test_pos)) right_tiles++;
        else break;
    }

    std::vector<float> offsets;
    if (n == 2)
    {
        offsets = { -0.75f, 0.75f };
    }
    else if (n == 3)
    {
        offsets = { -1.1f, 0.0f, 1.1f };
    }
    else // n >= 4
    {
        offsets = { -1.5f, -0.5f, 0.5f, 1.5f };
    }

    float min_safe = -1.0f - (DUNGEON_TILE * left_tiles) + 0.45f;
    float max_safe = 1.0f + (DUNGEON_TILE * right_tiles) - 0.45f;

    if (offsets.front() < min_safe)
    {
        float shift = min_safe - offsets.front();
        for (float& off : offsets) off += shift;
    }
    if (offsets.back() > max_safe)
    {
        float shift = offsets.back() - max_safe;
        for (float& off : offsets) off -= shift;
    }

    for (int i = 0; i < n && i < static_cast<int>(offsets.size()); ++i)
    {
        Vector3 pos = Vector3Add(combat_enemy_center, Vector3Scale(side_dir, offsets[i]));
        pos.y = active_enemies[i]->get_position().y;
        active_enemies[i]->set_position(pos);

        Vector3 to_cam = Vector3Normalize(Vector3Subtract(combat_player_pos, pos));
        to_cam.y = 0.0f;
        active_enemies[i]->set_forward(to_cam);
        active_enemies[i]->set_last_frame(0);
    }
}

void battle::join_battle(enemy* e, bool comes_from_left)
{
    if (e == nullptr || e->is_dead()) return;

    e->exp = this->exp;

    int max_capacity = get_max_combat_capacity();

    if (static_cast<int>(active_enemies.size()) >= max_capacity)
    {
        queued_enemies.push_back(e);
        int queue_slot = static_cast<int>(queued_enemies.size());
        const float DUNGEON_TILE = 2.0f;

        Vector3 queue_pos = Vector3Add(combat_enemy_center, Vector3Scale(combat_facing_dir, DUNGEON_TILE * queue_slot));
        queue_pos.y = e->get_position().y;
        e->set_position(queue_pos);

        Vector3 to_cam = Vector3Normalize(Vector3Subtract(combat_player_pos, queue_pos));
        to_cam.y = 0.0f;
        e->set_forward(to_cam);
        e->set_last_frame(0);

        gamestate::gameLogs.push_back(e->get_name() + " waits behind the opponent!");
    }
    else
    {
        if (comes_from_left)
        {
            active_enemies.insert(active_enemies.begin(), e);
        }
        else
        {
            active_enemies.push_back(e);
        }

        reposition_active_enemies();
        gamestate::gameLogs.push_back(e->get_name() + " joined the fight!");
    }
}

void battle::tick_reinforcements()
{
    Vector3 player_right = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));

    for (auto it = incoming_reinforcements.begin(); it != incoming_reinforcements.end(); )
    {
        it->rounds_to_arrive--;

        it->current_path_step = std::min(
            static_cast<int>(it->path.size()) - 1,
            it->current_path_step + it->steps_per_round
        );

        if (it->current_path_step >= 0 && it->current_path_step < static_cast<int>(it->path.size()))
        {
            GridPosition node = it->path[it->current_path_step];
            float world_x = (node.x - 4) * 0.5f + 0.25f;
            float world_z = node.z * 0.5f + 0.25f;
            Vector3 new_pos = { world_x, it->enemy_ptr->get_position().y, world_z };
            it->enemy_ptr->set_position(new_pos);

            Vector3 dir_to_p = Vector3Normalize(Vector3Subtract(combat_player_pos, new_pos));
            dir_to_p.y = 0.0f;
            if (Vector3Length(dir_to_p) > 0.0f)
            {
                it->enemy_ptr->set_forward(dir_to_p);
            }

            Vector3 to_cur = Vector3Subtract(new_pos, combat_player_pos);
            to_cur.y = 0.0f;
            it->is_approaching_from_left = (Vector3DotProduct(to_cur, player_right) < 0.0f);
        }

        if (it->rounds_to_arrive <= 0)
        {
            join_battle(it->enemy_ptr, it->is_approaching_from_left);
            it = incoming_reinforcements.erase(it);
        }
        else
        {
            gamestate::gameLogs.push_back(it->enemy_ptr->get_name() + " gets closer from the " +
                (it->is_approaching_from_left ? "left" : "right") + "! (" + std::to_string(it->rounds_to_arrive) + " rounds)");
            ++it;
        }
    }
}

void battle::handle_dead_enemies()
{
    std::vector<enemy*> dead_enemies;
    for (enemy* e : active_enemies)
    {
        if (e != nullptr && e->is_dead())
        {
            dead_enemies.push_back(e);
        }
    }

    if (dead_enemies.empty())
    {
        return;
    }

    for (enemy* e : dead_enemies)
    {
        p_ref.xp_from_enemy_dif = e->get_dif();
        p_ref.grant_xp();
        p_ref.check_level_up();

        if (exp != nullptr && exp->current_floor_id >= 0 && exp->current_floor_id < static_cast<int>(exp->floors.size()))
        {
            exp->spawn_object(ObjectSpawnInfo::create_dead_body(e, e->get_position()));
            auto& floor_enemies = exp->floors[exp->current_floor_id].active_enemies;
            std::erase(floor_enemies, e);
        }

        gamestate::gameLogs.push_back(e->get_name() + " has been defeated!");
        std::erase(pending_enemy_actions, e);
        std::erase(active_enemies, e);
    }

    size_t max_active = static_cast<size_t>(get_max_combat_capacity());
    while (!queued_enemies.empty() && active_enemies.size() < max_active)
    {
        enemy* next_in_line = queued_enemies.front();
        queued_enemies.erase(queued_enemies.begin());

        active_enemies.push_back(next_in_line);
        gamestate::gameLogs.push_back(next_in_line->get_name() + " steps forward from the queue into battle!");
    }

    const float DUNGEON_TILE = 2.0f;
    for (size_t q = 0; q < queued_enemies.size(); ++q)
    {
        Vector3 q_pos = Vector3Add(combat_enemy_center, Vector3Scale(combat_facing_dir, DUNGEON_TILE * (q + 1)));
        q_pos.y = queued_enemies[q]->get_position().y;
        queued_enemies[q]->set_position(q_pos);

        Vector3 to_cam = Vector3Normalize(Vector3Subtract(combat_player_pos, q_pos));
        to_cam.y = 0.0f;
        queued_enemies[q]->set_forward(to_cam);
        queued_enemies[q]->set_last_frame(0);
    }

    reposition_active_enemies();
}

bool battle::is_fight_finished() const
{
    if (active_enemies.empty() && queued_enemies.empty())
    {
        if (!incoming_reinforcements.empty() && incoming_reinforcements[0].rounds_to_arrive <= 1)
        {
            return false;
        }
        return true;
    }
    return false;
}

enemy* battle::get_current_target()
{
    for (enemy* e : active_enemies)
    {
        if (e != nullptr && !e->is_dead())
        {
            return e;
        }
    }
    return &e_ref;
}

int battle::player_turn()
{
    if (p_ref.is_dead())
    {
        return 0;
    }

    handle_dead_enemies();

    if (active_enemies.empty())
    {
        return 0;
    }

    if (click_cooldown)
    {
        player_cooldown -= GetFrameTime();
        if (player_cooldown <= 0.0)
        {
            click_cooldown = false;
        }
        return 0;
    }

    if (this->attack_clicked)
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            this->attack_clicked = false;
            for (enemy* e : active_enemies)
            {
                if (e) e->set_hovered_body_part(BodyPart::NONE);
            }
            return 0;
        }

        enemy* target_enemy = nullptr;
        BodyPart target_part = BodyPart::NONE;
        Vector3 target_hit_point = { 0.0f, 0.0f, 0.0f };

        for (enemy* e : active_enemies)
        {
            if (e == nullptr || e->is_dead()) continue;

            Vector3 to_cam = Vector3Normalize(Vector3Subtract(exp->camera.position, e->get_position()));
            to_cam.y = 0.0f;
            e->set_forward(to_cam);
            e->set_last_frame(0);

            Texture2D tex = (e->get_id() == 2) ? textures.pimpek : textures.ghoul;
            auto params = e->get_render_params(tex);

            BodyPart hovered = e->calculate_hovered_body_part(params.drawPos, params.targetWidth, params.targetHeight, exp->camera);
            e->set_hovered_body_part(hovered);

            if (hovered != BodyPart::NONE && target_enemy == nullptr)
            {
                target_enemy = e;
                target_part = hovered;

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
                target_hit_point = collision.hit ? collision.point : center;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && target_enemy != nullptr && target_part != BodyPart::NONE)
        {
            audio.play_sound(SoundID::PLAYER_HIT);
            auto [p_dmg, crit] = p_ref.calculate_dmg(p_ref.limbs.get_limb(BodyPart::RIGHT_ARM).damage);

            int final_dmg = target_enemy->take_damage(p_dmg, &p_ref, crit, p_ref.is_guard, target_part, this->exp, target_hit_point);

            this->waiting_for_enemy = true;
            this->click_cooldown = true;
            this->enemy_cooldown = 1.0f;
            this->pending_enemy_actions = this->active_enemies;
            this->turn_start_effects_processed = false;

            this->attack_clicked = false;
            for (enemy* e : active_enemies)
            {
                if (e) e->set_hovered_body_part(BodyPart::NONE);
            }

            return final_dmg;
        }
    }

    if (this->guard_clicked)
    {
        this->p_ref.player_guard();

        this->waiting_for_enemy = true;
        this->click_cooldown = true;
        this->enemy_cooldown = 1.0f;
        this->pending_enemy_actions = this->active_enemies;
        this->turn_start_effects_processed = false;

        this->guard_clicked = false;
        for (enemy* e : active_enemies)
        {
            if (e) e->set_hovered_body_part(BodyPart::NONE);
        }

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
        this->enemy_cooldown = (p_ref.queued_frame_count * p_ref.queued_frame_time) + 1.0f;
        this->pending_enemy_actions = this->active_enemies;
        this->turn_start_effects_processed = false;

        p_ref.spell_queued = false;
        for (enemy* e : active_enemies)
        {
            if (e) e->set_hovered_body_part(BodyPart::NONE);
        }
    }

    return 0;
}

int battle::enemy_turn()
{
    enemy_cooldown -= GetFrameTime();
    if (enemy_cooldown <= 0.0f)
    {
        if (!turn_start_effects_processed)
        {
            turn_start_effects_processed = true;
            p_ref.process_turn_start_effects(this->exp);
            if (p_ref.is_dead())
            {
                waiting_for_enemy = false;
                pending_enemy_actions.clear();
                return 0;
            }
        }

        while (!pending_enemy_actions.empty())
        {
            enemy* e = pending_enemy_actions.front();
            pending_enemy_actions.erase(pending_enemy_actions.begin());

            if (e != nullptr && !e->is_dead())
            {
                int dmg = e->execute_ai_turn(p_ref);
                if (p_ref.is_dead())
                {
                    waiting_for_enemy = false;
                    pending_enemy_actions.clear();
                    return dmg;
                }

                // Delay after this opponent's attack before next opponent or player turn
                enemy_cooldown = 1.0f;
                return dmg;
            }
        }

        // All active enemies have finished their attacks and delays
        waiting_for_enemy = false;
        turn_start_effects_processed = false;
        pending_enemy_actions.clear();

        tick_reinforcements();
        handle_dead_enemies();
    }

    return 0;
}
