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



static const Vector3 CARDINAL_DIRS[4] = {
    {  1.0f, 0.0f,  0.0f },
    { -1.0f, 0.0f,  0.0f },
    {  0.0f, 0.0f,  1.0f },
    {  0.0f, 0.0f, -1.0f }
};

battle::~battle()
{
    for (auto& rein : incoming_reinforcements)
    {
        if (rein.enemy_ptr != nullptr && !rein.enemy_ptr->is_dead())
        {
            rein.enemy_ptr->set_is_alerted(false);
            rein.enemy_ptr->set_is_moving(false);
        }
    }
}

void battle::initiate_fight_view()
{
    enemy& e = this->e_ref;
    player& p = this->p_ref;
    e.exp = this->exp;

    Vector3 e_pos = e.get_position();
    Vector3 p_pos = p.get_position();

    const float DUNGEON_TILE = 2.0f;

    // Centrujemy pozycje gracza na kafelku lochu
    float p_center_x = floorf(p_pos.x / DUNGEON_TILE) * DUNGEON_TILE + (DUNGEON_TILE * 0.5f);
    float p_center_z = floorf(p_pos.z / DUNGEON_TILE) * DUNGEON_TILE + (DUNGEON_TILE * 0.5f);
    Vector3 target_p_pos = { p_center_x, p_pos.y, p_center_z };

    if (!is_tile_walkable_at(target_p_pos))
    {
        target_p_pos = p_pos;
    }

    // Wektor wskazujacy od gracza bezposrednio na przeciwnika w momencie inicjacji (rzeczywiste pozycje)
    Vector3 raw_diff = Vector3Subtract(e_pos, p_pos);
    raw_diff.y = 0.0f;

    Vector3 norm_raw = { 0.0f, 0.0f, 1.0f };
    float raw_len = Vector3Length(raw_diff);
    if (raw_len > 0.0001f)
    {
        norm_raw = Vector3Scale(raw_diff, 1.0f / raw_len);
    }

    const Vector3 CARDINAL_DIRS[4] = {
        {  1.0f, 0.0f,  0.0f },
        { -1.0f, 0.0f,  0.0f },
        {  0.0f, 0.0f,  1.0f },
        {  0.0f, 0.0f, -1.0f }
    };

    Vector3 final_dir = { 0.0f, 0.0f, 0.0f };
    float best_score = -999999.0f;
    bool found_walkable = false;

    // Wybieramy kierunek cardinalny, ktory jest chodzalny i NAJBLIZSZY fizycznej pozycji wroga
    for (int i = 0; i < 4; i++)
    {
        Vector3 cand_dir = CARDINAL_DIRS[i];
        Vector3 cand_e_pos = Vector3Add(target_p_pos, Vector3Scale(cand_dir, DUNGEON_TILE));

        if (!is_tile_walkable_at(cand_e_pos) || (exp != nullptr && !exp->has_line_of_sight(target_p_pos, cand_e_pos)))
        {
            continue;
        }

        found_walkable = true;

        // Odleglosc kafelka walki do rzeczywistej pozycji wroga
        float dist_to_actual = Vector3Distance(cand_e_pos, e_pos);

        // Zgodnosc katowa (kierunek natarcia wroga)
        float dot = Vector3DotProduct(norm_raw, cand_dir);

        // Pojemnosc boczna (dodatkowe miejsce dla posilkow) - wylacznie jako minimalny tie-breaker
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

        // Kluczowe: dot (kat natarcia) i dist (bliskosc fizycznej pozycji) bezwzglednie dominuja.
        // capacity daje maksymalnie 0.4 pkt, wiec nigdy nie obroci widoku o 90 stopni wzdluz innej sciezki.
        float score = (dot * 100.0f) - (dist_to_actual * 25.0f) + (capacity * 0.1f);

        if (score > best_score)
        {
            best_score = score;
            final_dir = cand_dir;
        }
    }

    if (!found_walkable)
    {
        target_p_pos = p_pos;
        for (int i = 0; i < 4; i++)
        {
            Vector3 cand_dir = CARDINAL_DIRS[i];
            Vector3 cand_e_pos = Vector3Add(target_p_pos, Vector3Scale(cand_dir, DUNGEON_TILE));
            if (is_tile_walkable_at(cand_e_pos) && (exp == nullptr || exp->has_line_of_sight(target_p_pos, cand_e_pos)))
            {
                float dot = Vector3DotProduct(norm_raw, cand_dir);
                float dist_to_actual = Vector3Distance(cand_e_pos, e_pos);
                float score = (dot * 100.0f) - (dist_to_actual * 25.0f);
                if (score > best_score)
                {
                    best_score = score;
                    final_dir = cand_dir;
                    found_walkable = true;
                }
            }
        }
        if (!found_walkable)
        {
            if (fabsf(raw_diff.x) >= fabsf(raw_diff.z))
            {
                final_dir.x = (raw_diff.x >= 0.0f) ? 1.0f : -1.0f;
            }
            else
            {
                final_dir.z = (raw_diff.z >= 0.0f) ? 1.0f : -1.0f;
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
    unconfirmed_reinforcements.clear();
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

    const float DUNGEON_TILE = 2.0f;

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
        float dist_to_battle = Vector3Distance(other_pos, combat_player_pos);
        if (dist_to_battle > 16.0f)
        {
            other_e->set_is_alerted(false);
            continue;
        }

        // If other_e is already adjacent to the player (distance <= 3.0f, ~1 dungeon tile)
        if (dist_to_battle <= 3.0f && exp != nullptr && exp->has_line_of_sight(combat_player_pos, other_pos))
        {
            Vector3 raw_diff = Vector3Subtract(other_pos, combat_player_pos);
            raw_diff.y = 0.0f;
            float raw_len = Vector3Length(raw_diff);
            Vector3 norm_diff = (raw_len > 0.001f) ? Vector3Scale(raw_diff, 1.0f / raw_len) : combat_facing_dir;

            Vector3 best_card = combat_facing_dir;
            float best_dot = -999.0f;
            bool found_cand = false;

            for (int i = 0; i < 4; ++i)
            {
                Vector3 test_cand = Vector3Add(combat_player_pos, Vector3Scale(CARDINAL_DIRS[i], DUNGEON_TILE));
                test_cand.y = other_pos.y;
                if (!is_tile_walkable_at(test_cand) || !exp->has_line_of_sight(combat_player_pos, test_cand))
                {
                    continue;
                }

                float d = Vector3DotProduct(norm_diff, CARDINAL_DIRS[i]);
                if (d > best_dot)
                {
                    best_dot = d;
                    best_card = CARDINAL_DIRS[i];
                    found_cand = true;
                }
            }

            if (!found_cand)
            {
                continue;
            }

            Vector3 cand_pos = Vector3Add(combat_player_pos, Vector3Scale(best_card, DUNGEON_TILE));
            cand_pos.y = other_pos.y;

            if (is_tile_walkable_at(cand_pos) && exp->has_line_of_sight(combat_player_pos, cand_pos))
            {
                other_e->exp = this->exp;
                other_e->set_position(cand_pos);
                Vector3 to_p = Vector3Normalize(Vector3Subtract(combat_player_pos, cand_pos));
                to_p.y = 0.0f;
                other_e->set_forward(to_p);
                other_e->set_last_frame(0);

                float dot_front = Vector3DotProduct(best_card, combat_facing_dir);
                if (dot_front > 0.8f)
                {
                    int front_count = 0;
                    for (enemy* act : active_enemies)
                    {
                        if (act == nullptr || act->is_dead()) continue;
                        Vector3 d = Vector3Normalize(Vector3Subtract(act->get_position(), combat_player_pos));
                        d.y = 0.0f;
                        if (Vector3DotProduct(d, combat_facing_dir) > 0.8f) front_count++;
                    }

                    if (front_count < get_max_combat_capacity())
                    {
                        active_enemies.push_back(other_e);
                        gamestate::gameLogs.push_back(other_e->get_name() + " joined the fight!");
                    }
                    else
                    {
                        queued_enemies.push_back(other_e);
                        Vector3 q_pos = Vector3Add(combat_enemy_center, Vector3Scale(combat_facing_dir, DUNGEON_TILE * queued_enemies.size()));
                        q_pos.y = other_pos.y;
                        other_e->set_position(q_pos);
                        gamestate::gameLogs.push_back(other_e->get_name() + " waits behind the opponent!");
                    }
                }
                else
                {
                    // Directly join battle on flank!
                    active_enemies.push_back(other_e);
                    gamestate::gameLogs.push_back(other_e->get_name() + " attacks from the flank!");
                }
                continue;
            }
        }

        int e_grid_x = static_cast<int>(std::floor(other_pos.x / nodeSize)) + 4;
        int e_grid_z = static_cast<int>(std::floor(other_pos.z / nodeSize));

        std::vector<GridPosition> path = exp->find_path(e_grid_x, e_grid_z, p_grid_x, p_grid_z, other_e, true);

        if (!path.empty())
        {
            ReinforcementEnemy rein;
            rein.enemy_ptr = other_e;
            rein.path = path;
            rein.current_path_step = 0;
            rein.steps_per_round = (other_e->get_name() == "Pimpek") ? 8 : 4;

            const float COMBAT_RADIUS = 2.4f;
            int steps_to_combat = static_cast<int>(path.size());
            for (size_t p_idx = 0; p_idx < path.size(); ++p_idx)
            {
                float wx = (path[p_idx].x - 4) * 0.5f + 0.25f;
                float wz = path[p_idx].z * 0.5f + 0.25f;
                Vector3 step_pt = { wx, combat_player_pos.y, wz };
                float d = Vector3Distance(step_pt, combat_player_pos);
                if (d <= COMBAT_RADIUS && (exp == nullptr || exp->has_line_of_sight(step_pt, combat_player_pos)))
                {
                    steps_to_combat = static_cast<int>(p_idx + 1);
                    break;
                }
            }

            int calculated_rounds = static_cast<int>(std::ceil(static_cast<float>(steps_to_combat) / static_cast<float>(rein.steps_per_round)));
            
            const int MAX_REINFORCEMENT_ROUNDS = 6;
            if (calculated_rounds > MAX_REINFORCEMENT_ROUNDS)
            {
                other_e->set_is_alerted(false);
                continue;
            }

            rein.rounds_to_arrive = std::max(1, calculated_rounds);

            Vector3 player_right = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));
            Vector3 to_e = Vector3Subtract(other_pos, combat_player_pos);
            to_e.y = 0.0f;
            rein.is_approaching_from_left = (Vector3DotProduct(to_e, player_right) < 0.0f);

            incoming_reinforcements.push_back(rein);
        }
        else
        {
            other_e->set_is_alerted(false);
        }
    }

    reposition_active_enemies();
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
            gz >= node.room_y && gz < node.room_y + node.room_height)
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

    // Player is in a corridor
    const float DUNGEON_TILE = 2.0f;
    Vector3 side_dir = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));

    bool left_blocked = !is_tile_walkable_at(Vector3Subtract(combat_player_pos, Vector3Scale(side_dir, DUNGEON_TILE)));
    bool right_blocked = !is_tile_walkable_at(Vector3Add(combat_player_pos, Vector3Scale(side_dir, DUNGEON_TILE)));

    return (left_blocked && right_blocked);
}

int battle::get_max_combat_capacity() const
{
    if (exp == nullptr) return 1;

    // Tactical positioning
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

    const float DUNGEON_TILE = 2.0f;
    const Vector3 CARDINAL_DIRS[4] = {
        {  1.0f, 0.0f,  0.0f },
        { -1.0f, 0.0f,  0.0f },
        {  0.0f, 0.0f,  1.0f },
        {  0.0f, 0.0f, -1.0f }
    };

    // Group active enemies by their closest cardinal direction from combat_player_pos
    std::vector<enemy*> dir_groups[4];

    for (enemy* e : active_enemies)
    {
        if (e == nullptr || e->is_dead()) continue;

        Vector3 diff = Vector3Subtract(e->get_position(), combat_player_pos);
        diff.y = 0.0f;

        int best_idx = -1;
        float best_dot = -999.0f;
        float diff_len = Vector3Length(diff);
        Vector3 norm_diff = (diff_len > 0.001f) ? Vector3Scale(diff, 1.0f / diff_len) : combat_facing_dir;

        for (int i = 0; i < 4; ++i)
        {
            Vector3 test_tile = Vector3Add(combat_player_pos, Vector3Scale(CARDINAL_DIRS[i], DUNGEON_TILE));
            if (!is_tile_walkable_at(test_tile) || (exp != nullptr && !exp->has_line_of_sight(combat_player_pos, test_tile)))
            {
                continue;
            }

            float d = Vector3DotProduct(norm_diff, CARDINAL_DIRS[i]);
            if (d > best_dot)
            {
                best_dot = d;
                best_idx = i;
            }
        }

        if (best_idx == -1)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (Vector3DotProduct(combat_facing_dir, CARDINAL_DIRS[i]) > 0.8f)
                {
                    best_idx = i;
                    break;
                }
            }
            if (best_idx == -1) best_idx = 0;
        }
        dir_groups[best_idx].push_back(e);
    }

    // Position each group along its respective cardinal flank
    for (int k = 0; k < 4; ++k)
    {
        auto& group = dir_groups[k];
        if (group.empty()) continue;

        Vector3 dir = CARDINAL_DIRS[k];
        Vector3 group_center = Vector3Add(combat_player_pos, Vector3Scale(dir, DUNGEON_TILE));
        int count = static_cast<int>(group.size());

        if (count == 1)
        {
            Vector3 pos = group_center;
            pos.y = group[0]->get_position().y;
            group[0]->set_position(pos);

            Vector3 to_cam = Vector3Normalize(Vector3Subtract(combat_player_pos, pos));
            to_cam.y = 0.0f;
            group[0]->set_forward(to_cam);
            group[0]->set_last_frame(0);
        }
        else
        {
            Vector3 side_dir = Vector3Normalize(Vector3CrossProduct(dir, { 0.0f, 1.0f, 0.0f }));

            int left_tiles = 0;
            for (int step = 1; step <= 3; ++step)
            {
                Vector3 test_pos = Vector3Subtract(group_center, Vector3Scale(side_dir, DUNGEON_TILE * step));
                if (is_tile_walkable_at(test_pos)) left_tiles++;
                else break;
            }

            int right_tiles = 0;
            for (int step = 1; step <= 3; ++step)
            {
                Vector3 test_pos = Vector3Add(group_center, Vector3Scale(side_dir, DUNGEON_TILE * step));
                if (is_tile_walkable_at(test_pos)) right_tiles++;
                else break;
            }

            std::vector<float> offsets;
            if (count == 2)
            {
                offsets = { -0.75f, 0.75f };
            }
            else if (count == 3)
            {
                offsets = { -1.1f, 0.0f, 1.1f };
            }
            else
            {
                offsets = { -1.5f, -0.5f, 0.5f, 1.5f };
            }

            float min_safe = -1.0f - (DUNGEON_TILE * left_tiles) + 0.45f;
            float max_safe = 1.0f + (DUNGEON_TILE * right_tiles) - 0.45f;

            float current_span = offsets.back() - offsets.front();
            float safe_span = max_safe - min_safe;
            if (current_span > safe_span && current_span > 0.001f)
            {
                float scale = safe_span / current_span;
                for (float& off : offsets) off *= scale;
            }

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

            for (int i = 0; i < count && i < static_cast<int>(offsets.size()); ++i)
            {
                Vector3 pos = Vector3Add(group_center, Vector3Scale(side_dir, offsets[i]));
                pos.y = group[i]->get_position().y;
                group[i]->set_position(pos);

                Vector3 to_cam = Vector3Normalize(Vector3Subtract(combat_player_pos, pos));
                to_cam.y = 0.0f;
                group[i]->set_forward(to_cam);
                group[i]->set_last_frame(0);
            }
        }
    }
}

void battle::join_battle(enemy* e, bool comes_from_left)
{
    if (e == nullptr || e->is_dead()) return;

    e->exp = this->exp;

    const float DUNGEON_TILE = 2.0f;
    const Vector3 CARDINAL_DIRS[4] = {
        {  1.0f, 0.0f,  0.0f },
        { -1.0f, 0.0f,  0.0f },
        {  0.0f, 0.0f,  1.0f },
        {  0.0f, 0.0f, -1.0f }
    };

    Vector3 raw_diff = Vector3Subtract(e->get_position(), combat_player_pos);
    raw_diff.y = 0.0f;
    Vector3 best_card = combat_facing_dir;
    float best_dot = -999.0f;
    float diff_len = Vector3Length(raw_diff);
    Vector3 norm_diff = (diff_len > 0.001f) ? Vector3Scale(raw_diff, 1.0f / diff_len) : combat_facing_dir;

    for (int i = 0; i < 4; ++i)
    {
        Vector3 test_cand = Vector3Add(combat_player_pos, Vector3Scale(CARDINAL_DIRS[i], DUNGEON_TILE));
        if (!is_tile_walkable_at(test_cand) || (exp != nullptr && !exp->has_line_of_sight(combat_player_pos, test_cand)))
        {
            continue;
        }

        float d = Vector3DotProduct(norm_diff, CARDINAL_DIRS[i]);
        if (d > best_dot)
        {
            best_dot = d;
            best_card = CARDINAL_DIRS[i];
        }
    }

    float dot_front = Vector3DotProduct(best_card, combat_facing_dir);
    if (dot_front <= 0.8f)
    {
        Vector3 test_flank = Vector3Add(combat_player_pos, Vector3Scale(best_card, DUNGEON_TILE));
        test_flank.y = e->get_position().y;
        if (!is_tile_walkable_at(test_flank) || (exp != nullptr && !exp->has_line_of_sight(combat_player_pos, test_flank)))
        {
            best_card = combat_facing_dir;
            dot_front = 1.0f;
        }
    }
    if (dot_front > 0.8f)
    {
        int max_capacity = get_max_combat_capacity();
        int front_count = 0;
        for (enemy* act : active_enemies)
        {
            if (act == nullptr || act->is_dead()) continue;
            Vector3 d = Vector3Normalize(Vector3Subtract(act->get_position(), combat_player_pos));
            d.y = 0.0f;
            if (Vector3DotProduct(d, combat_facing_dir) > 0.8f) front_count++;
        }

        if (front_count >= max_capacity)
        {
            queued_enemies.push_back(e);
            int queue_slot = static_cast<int>(queued_enemies.size());
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
    else
    {
        // Flank
        bool flank_occupied = false;
        for (enemy* act : active_enemies)
        {
            if (act == nullptr || act->is_dead()) continue;
            Vector3 d = Vector3Normalize(Vector3Subtract(act->get_position(), combat_player_pos));
            d.y = 0.0f;
            if (Vector3DotProduct(d, best_card) > 0.8f)
            {
                flank_occupied = true;
                break;
            }
        }

        if (!flank_occupied)
        {
            Vector3 flank_pos = Vector3Add(combat_player_pos, Vector3Scale(best_card, DUNGEON_TILE));
            flank_pos.y = e->get_position().y;
            e->set_position(flank_pos);

            Vector3 to_cam = Vector3Normalize(Vector3Subtract(combat_player_pos, flank_pos));
            to_cam.y = 0.0f;
            e->set_forward(to_cam);
            e->set_last_frame(0);

            active_enemies.push_back(e);
            reposition_active_enemies();
            gamestate::gameLogs.push_back(e->get_name() + " attacks from the flank!");
        }
        else
        {
            queued_enemies.push_back(e);
            Vector3 queue_pos = Vector3Add(combat_player_pos, Vector3Scale(best_card, DUNGEON_TILE * 2.0f));
            queue_pos.y = e->get_position().y;
            e->set_position(queue_pos);

            Vector3 to_cam = Vector3Normalize(Vector3Subtract(combat_player_pos, queue_pos));
            to_cam.y = 0.0f;
            e->set_forward(to_cam);
            e->set_last_frame(0);

            gamestate::gameLogs.push_back(e->get_name() + " waits on the flank!");
        }
    }
}

void battle::tick_reinforcements()
{
    Vector3 player_right = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));
    const float COMBAT_RADIUS = 2.4f;

    for (auto it = incoming_reinforcements.begin(); it != incoming_reinforcements.end(); )
    {
        it->rounds_to_arrive--;

        int next_step = std::min(
            static_cast<int>(it->path.size()) - 1,
            it->current_path_step + it->steps_per_round
        );

        bool reached_combat = false;

        if (next_step >= 0 && next_step < static_cast<int>(it->path.size()))
        {
            GridPosition node = it->path[next_step];
            float world_x = (node.x - 4) * 0.5f + 0.25f;
            float world_z = node.z * 0.5f + 0.25f;
            Vector3 test_pos = { world_x, it->enemy_ptr->get_position().y, world_z };

            float dist_to_player = Vector3Distance(test_pos, combat_player_pos);
            bool has_los = (exp == nullptr || exp->has_line_of_sight(test_pos, combat_player_pos));
            if ((dist_to_player <= COMBAT_RADIUS || next_step == static_cast<int>(it->path.size()) - 1) && has_los)
            {
                reached_combat = true;
            }
            else
            {
                it->current_path_step = next_step;
                it->enemy_ptr->set_position(test_pos);

                Vector3 dir_to_p = Vector3Normalize(Vector3Subtract(combat_player_pos, test_pos));
                dir_to_p.y = 0.0f;
                if (Vector3Length(dir_to_p) > 0.0f)
                {
                    it->enemy_ptr->set_forward(dir_to_p);
                }

                Vector3 to_cur = Vector3Subtract(test_pos, combat_player_pos);
                to_cur.y = 0.0f;
                it->is_approaching_from_left = (Vector3DotProduct(to_cur, player_right) < 0.0f);
            }
        }
        else
        {
            reached_combat = false;
        }

        if (reached_combat)
        {
            join_battle(it->enemy_ptr, it->is_approaching_from_left);
            unconfirmed_reinforcements.push_back(it->enemy_ptr);
            it = incoming_reinforcements.erase(it);
        }
        else
        {
            if (it->rounds_to_arrive <= 0)
            {
                it->rounds_to_arrive = 1;
            }
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

        // Find which direction the dead enemy was in
        Vector3 dead_dir = Vector3Normalize(Vector3Subtract(e->get_position(), combat_player_pos));
        dead_dir.y = 0.0f;

        std::erase(active_enemies, e);

        // Try to promote a queued enemy from the same direction
        auto it = std::find_if(queued_enemies.begin(), queued_enemies.end(), [&](enemy* q) {
            if (q == nullptr || q->is_dead()) return false;
            Vector3 q_dir = Vector3Normalize(Vector3Subtract(q->get_position(), combat_player_pos));
            q_dir.y = 0.0f;
            return Vector3DotProduct(q_dir, dead_dir) > 0.8f;
        });

        if (it != queued_enemies.end())
        {
            enemy* next_in_line = *it;
            queued_enemies.erase(it);
            active_enemies.push_back(next_in_line);
            gamestate::gameLogs.push_back(next_in_line->get_name() + " steps into battle!");
        }
        else if (!queued_enemies.empty() && active_enemies.empty())
        {
            enemy* next_in_line = queued_enemies.front();
            queued_enemies.erase(queued_enemies.begin());
            active_enemies.push_back(next_in_line);
            gamestate::gameLogs.push_back(next_in_line->get_name() + " steps into battle!");
        }
    }

    reposition_active_enemies();

    // If active_enemies has living enemies, but none in front of current camera facing:
    // Auto-turn towards the nearest living enemy so the player is not facing an empty wall
    bool facing_any = false;
    for (enemy* e : active_enemies)
    {
        if (e != nullptr && !e->is_dead())
        {
            Vector3 d = Vector3Normalize(Vector3Subtract(e->get_position(), combat_player_pos));
            d.y = 0.0f;
            if (Vector3DotProduct(d, combat_facing_dir) > 0.5f)
            {
                facing_any = true;
                break;
            }
        }
    }

    if (!facing_any && !active_enemies.empty())
    {
        enemy* next_e = active_enemies[0];
        Vector3 d = Vector3Normalize(Vector3Subtract(next_e->get_position(), combat_player_pos));
        set_combat_facing(d);
    }
}

void battle::set_combat_facing(Vector3 new_dir)
{
    new_dir.y = 0.0f;
    if (fabsf(new_dir.x) >= fabsf(new_dir.z))
    {
        new_dir.x = (new_dir.x >= 0.0f) ? 1.0f : -1.0f;
        new_dir.z = 0.0f;
    }
    else
    {
        new_dir.x = 0.0f;
        new_dir.z = (new_dir.z >= 0.0f) ? 1.0f : -1.0f;
    }

    combat_facing_dir = new_dir;
    const float DUNGEON_TILE = 2.0f;
    float cam_y = (exp != nullptr) ? exp->camera.target.y : combat_enemy_center.y;
    combat_enemy_center = Vector3Add(combat_player_pos, Vector3Scale(combat_facing_dir, DUNGEON_TILE));
    combat_enemy_center.y = cam_y;

    if (exp != nullptr)
    {
        exp->camera.position.x = combat_player_pos.x;
        exp->camera.position.z = combat_player_pos.z;
        exp->camera.target = combat_enemy_center;
    }

    for (enemy* e : active_enemies)
    {
        if (e != nullptr)
        {
            e->set_hovered_body_part(BodyPart::NONE);
        }
    }
}

bool battle::is_direction_valid(Vector3 dir) const
{
    if (exp == nullptr) return false;

    const float DUNGEON_TILE = 2.0f;
    Vector3 tile_pos = Vector3Add(combat_player_pos, Vector3Scale(dir, DUNGEON_TILE));

    if (is_tile_walkable_at(tile_pos))
    {
        return true;
    }

    for (enemy* e : active_enemies)
    {
        if (e != nullptr && !e->is_dead())
        {
            Vector3 diff = Vector3Subtract(e->get_position(), combat_player_pos);
            diff.y = 0.0f;
            if (Vector3Length(diff) > 0.1f && Vector3DotProduct(Vector3Normalize(diff), dir) > 0.8f)
            {
                return true;
            }
        }
    }

    for (enemy* e : queued_enemies)
    {
        if (e != nullptr && !e->is_dead())
        {
            Vector3 diff = Vector3Subtract(e->get_position(), combat_player_pos);
            diff.y = 0.0f;
            if (Vector3Length(diff) > 0.1f && Vector3DotProduct(Vector3Normalize(diff), dir) > 0.8f)
            {
                return true;
            }
        }
    }

    return false;
}

Vector3 battle::get_next_turn_right_dir() const
{
    Vector3 player_right = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));
    Vector3 dir_behind = Vector3Scale(combat_facing_dir, -1.0f);
    Vector3 dir_left = Vector3Scale(player_right, -1.0f);

    Vector3 candidates[3] = { player_right, dir_behind, dir_left };

    for (int i = 0; i < 3; ++i)
    {
        if (is_direction_valid(candidates[i]))
        {
            if (i == 2)
            {
                // Left is the only open direction; right turn cannot reach it directly
                return { 0.0f, 0.0f, 0.0f };
            }
            return candidates[i];
        }
    }

    return { 0.0f, 0.0f, 0.0f };
}

Vector3 battle::get_next_turn_left_dir() const
{
    Vector3 player_right = Vector3Normalize(Vector3CrossProduct(combat_facing_dir, { 0.0f, 1.0f, 0.0f }));
    Vector3 dir_behind = Vector3Scale(combat_facing_dir, -1.0f);
    Vector3 dir_left = Vector3Scale(player_right, -1.0f);

    Vector3 candidates[3] = { dir_left, dir_behind, player_right };

    for (int i = 0; i < 3; ++i)
    {
        if (is_direction_valid(candidates[i]))
        {
            if (i == 2)
            {
                // Right is the only open direction; left turn cannot reach it directly
                return { 0.0f, 0.0f, 0.0f };
            }
            return candidates[i];
        }
    }

    return { 0.0f, 0.0f, 0.0f };
}

bool battle::can_turn_left() const
{
    Vector3 next_dir = get_next_turn_left_dir();
    return (Vector3Length(next_dir) > 0.1f);
}

bool battle::can_turn_right() const
{
    Vector3 next_dir = get_next_turn_right_dir();
    return (Vector3Length(next_dir) > 0.1f);
}

void battle::turn_camera_left()
{
    Vector3 next_dir = get_next_turn_left_dir();
    if (Vector3Length(next_dir) > 0.1f)
    {
        set_combat_facing(next_dir);
    }
}

void battle::turn_camera_right()
{
    Vector3 next_dir = get_next_turn_right_dir();
    if (Vector3Length(next_dir) > 0.1f)
    {
        set_combat_facing(next_dir);
    }
}

bool battle::has_enemy_on_left() const
{
    if (!can_turn_left()) return false;

    Vector3 next_dir = get_next_turn_left_dir();
    for (enemy* e : active_enemies)
    {
        if (e == nullptr || e->is_dead()) continue;
        Vector3 diff = Vector3Subtract(e->get_position(), combat_player_pos);
        diff.y = 0.0f;
        if (Vector3Length(diff) < 0.1f) continue;
        Vector3 dir = Vector3Normalize(diff);

        if (Vector3DotProduct(dir, next_dir) > 0.6f) return true;
    }
    return false;
}

bool battle::has_enemy_on_right() const
{
    if (!can_turn_right()) return false;

    Vector3 next_dir = get_next_turn_right_dir();
    for (enemy* e : active_enemies)
    {
        if (e == nullptr || e->is_dead()) continue;
        Vector3 diff = Vector3Subtract(e->get_position(), combat_player_pos);
        diff.y = 0.0f;
        if (Vector3Length(diff) < 0.1f) continue;
        Vector3 dir = Vector3Normalize(diff);

        if (Vector3DotProduct(dir, next_dir) > 0.6f) return true;
    }
    return false;
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
    // Prefer enemy currently in front of the camera
    for (enemy* e : active_enemies)
    {
        if (e != nullptr && !e->is_dead())
        {
            Vector3 diff = Vector3Subtract(e->get_position(), combat_player_pos);
            diff.y = 0.0f;
            if (Vector3Length(diff) > 0.1f && Vector3DotProduct(Vector3Normalize(diff), combat_facing_dir) > 0.5f)
            {
                return e;
            }
        }
    }

    // Fallback: any living active enemy
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

    // Keyboard rotation for camera (A/D or Left/Right or Q/E)
    if (!waiting_for_enemy && !click_cooldown)
    {
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_Q))
        {
            turn_camera_left();
        }
        else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_E))
        {
            turn_camera_right();
        }
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

            // Only allow targeting enemies in front of the camera (facing direction)
            Vector3 to_e = Vector3Subtract(e->get_position(), exp->camera.position);
            to_e.y = 0.0f;
            if (Vector3DotProduct(Vector3Normalize(to_e), combat_facing_dir) < 0.4f)
            {
                e->set_hovered_body_part(BodyPart::NONE);
                continue;
            }

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
    float dt = GetFrameTime();

    const float FLASH_DURATION = 0.30f;
    const float STRIKE_DELAY   = 0.45f;

    if (!turn_start_effects_processed)
    {
        enemy_cooldown -= dt;
        if (enemy_cooldown > 0.0f)
        {
            return 0;
        }

        turn_start_effects_processed = true;
        p_ref.process_turn_start_effects(this->exp);
        if (p_ref.is_dead())
        {
            waiting_for_enemy = false;
            pending_enemy_actions.clear();
            return 0;
        }

        enemy_phase = EnemyAttackPhase::INIT_NEXT_ENEMY;
    }

    switch (enemy_phase)
    {
    case EnemyAttackPhase::INIT_NEXT_ENEMY:
    {
        while (!pending_enemy_actions.empty())
        {
            current_turn_enemy = pending_enemy_actions.front();
            pending_enemy_actions.erase(pending_enemy_actions.begin());

            if (current_turn_enemy != nullptr && !current_turn_enemy->is_dead())
            {
                // Auto-face camera toward attacking enemy so player sees their limb flash and strike
                Vector3 to_att = Vector3Subtract(current_turn_enemy->get_position(), combat_player_pos);
                to_att.y = 0.0f;
                if (Vector3Length(to_att) > 0.1f)
                {
                    Vector3 att_dir = Vector3Normalize(to_att);
                    if (Vector3DotProduct(att_dir, combat_facing_dir) < 0.7f)
                    {
                        set_combat_facing(att_dir);
                    }
                }

                if (!global_fx.is_playing && p_ref.queued_damage > 0)
                {
                    current_turn_enemy->take_damage((int)p_ref.queued_damage, &p_ref, false, false, p_ref.queued_hit_part, this->exp);
                    p_ref.queued_damage = 0.0f;
                    p_ref.queued_hit_part = BodyPart::NONE;
                }

                if (current_turn_enemy->is_dead()) continue;

                current_turn_enemy->process_turn_start_effects(this->exp);
                if (current_turn_enemy->is_dead()) continue;

                current_enemy_attacks = current_turn_enemy->decide_attacks();
                current_attack_index = 0;

                if (!current_enemy_attacks.empty())
                {
                    BodyPart first_part = current_enemy_attacks[0];
                    current_turn_enemy->set_hovered_body_part(first_part);

                    enemy_phase = EnemyAttackPhase::FLASH_LIMB;
                    attack_phase_timer = FLASH_DURATION;
                    return 0;
                }
            }
        }

        waiting_for_enemy = false;
        turn_start_effects_processed = false;
        pending_enemy_actions.clear();
        current_turn_enemy = nullptr;
        current_enemy_attacks.clear();
        p_ref.is_guard = false;

        tick_reinforcements();
        handle_dead_enemies();
        return 0;
    }

    case EnemyAttackPhase::FLASH_LIMB:
    {
        attack_phase_timer -= dt;
        if (attack_phase_timer <= 0.0f)
        {
            if (current_turn_enemy != nullptr && !current_turn_enemy->is_dead())
            {
                current_turn_enemy->set_hovered_body_part(BodyPart::NONE);

                if (current_attack_index < current_enemy_attacks.size())
                {
                    BodyPart attacking_part = current_enemy_attacks[current_attack_index];
                    current_turn_enemy->perform_strike(attacking_part, p_ref);
                }
            }

            if (p_ref.is_dead())
            {
                if (current_turn_enemy) current_turn_enemy->set_hovered_body_part(BodyPart::NONE);
                waiting_for_enemy = false;
                pending_enemy_actions.clear();
                return 0;
            }

            enemy_phase = EnemyAttackPhase::RECOVERY_PAUSE;
            attack_phase_timer = STRIKE_DELAY;
        }
        return 0;
    }

    case EnemyAttackPhase::RECOVERY_PAUSE:
    {
        attack_phase_timer -= dt;
        if (attack_phase_timer <= 0.0f)
        {
            current_attack_index++;

            if (current_turn_enemy != nullptr && !current_turn_enemy->is_dead() &&
                current_attack_index < current_enemy_attacks.size())
            {
                BodyPart next_part = current_enemy_attacks[current_attack_index];
                current_turn_enemy->set_hovered_body_part(next_part);

                enemy_phase = EnemyAttackPhase::FLASH_LIMB;
                attack_phase_timer = FLASH_DURATION;
            }
            else
            {
                if (current_turn_enemy)
                {
                    current_turn_enemy->set_hovered_body_part(BodyPart::NONE);
                }
                current_turn_enemy = nullptr;
                current_enemy_attacks.clear();
                current_attack_index = 0;

                enemy_phase = EnemyAttackPhase::INIT_NEXT_ENEMY;
            }
        }
        return 0;
    }

    default:
        break;
    }

    return 0;
}
