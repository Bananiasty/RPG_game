#include "character.h"
#include "raylib.h"
#include "raymath.h"
#include "gamestates.h"
#include "struct.h"
#include "AudioManager.h"



void exploration::update_enemies()
{

    float dt = GetFrameTime();
    float detectionRange = 8.0f;
    float enemySpeed = 0.0f;

    auto& current_floor = floors[current_floor_id];

    for (enemy* e : current_floor.active_enemies)
    {
        if (!e || e->is_dead())
        {
            continue;
        }

        Vector3 enemyPos = e->get_position();
        float distance = Vector3Distance(enemyPos, bohater.position);

        if (distance <= detectionRange)
        {
            if (!e->get_is_alerted())
            {
                e->set_is_alerted(true);
                if (!audio.is_sound_playing(SoundID::ENEMY_DETECT))
                {
                    audio.play_sound(SoundID::ENEMY_DETECT);
                }
            }

            if (!e->is_moving())
            {
                apply_pathfinding(e);
            }

            if (e->is_moving())
            {
                Vector3 target = e->get_target_tile();
				if (e->get_name() == "Ghoul")
				{
                    enemySpeed = 1.5f;        
				}
                if (e->get_name() == "Pimpek")
                {
                    enemySpeed = 3.0f;
                }
                Vector3 nextPos = Vector3MoveTowards(enemyPos, target, enemySpeed * dt);

                Vector3 stary_cam_pos = camera.position;
                Vector3 stary_cam_target = camera.target;

                camera.position = nextPos;
                camera.target = nextPos;

                apply_collision(enemyPos);

                Vector3 finalPos = camera.position;

                for (enemy* other : current_floor.active_enemies)
                {
                    if (other != nullptr && other != e && !other->is_dead())
                    {
                        Vector3 otherPos = other->get_position();
                        float d = Vector3Distance(finalPos, otherPos);
                        const float minSeparation = 0.8f;
                        if (d < minSeparation && d > 0.0001f)
                        {
                            Vector3 pushDir = Vector3Normalize(Vector3Subtract(finalPos, otherPos));
                            float pushAmount = (minSeparation - d) * 0.5f;
                            Vector3 testSeparated = finalPos;
                            testSeparated.x += pushDir.x * pushAmount;
                            testSeparated.z += pushDir.z * pushAmount;

                            int check_x = static_cast<int>(testSeparated.x / 2.0f) + 1;
                            int check_z = static_cast<int>(testSeparated.z / 2.0f);
                            if (is_walkable(check_x, check_z))
                            {
                                finalPos = testSeparated;
                            }
                        }
                    }
                }

                e->set_position(finalPos);

                camera.position = stary_cam_pos;
                camera.target = stary_cam_target;

                float distToTarget = Vector3Distance(finalPos, target);
                float movedDistance = Vector3Distance(enemyPos, finalPos);

                if (distToTarget < 0.05f || movedDistance < 0.0001f)
                {
                    e->set_position(finalPos);
                    e->set_is_moving(false);
                }
            }
        }
    }
}

BodyPart ghoul::calculate_hovered_body_part(Vector3 drawPos, float targetWidth, float targetHeight, Camera3D camera)
{
    Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);

    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
    Vector3 up = Vector3Normalize(camera.up);

    float halfW = targetWidth * 0.5f;
    float halfH = targetHeight * 0.5f;

    Vector3 topLeft = Vector3Add(Vector3Subtract(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
    Vector3 topRight = Vector3Add(Vector3Add(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
    Vector3 bottomRight = Vector3Subtract(Vector3Add(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
    Vector3 bottomLeft = Vector3Subtract(Vector3Subtract(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));

    RayCollision collision = GetRayCollisionQuad(ray, topLeft, topRight, bottomRight, bottomLeft);

    if (collision.hit)
    {

        Vector3 bottomLeft = Vector3Subtract(Vector3Subtract(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
        Vector3 rel = Vector3Subtract(collision.point, bottomLeft);

        float xOnQuad = Vector3DotProduct(rel, right);
        float yOnQuad = Vector3DotProduct(rel, up); 

        float localX = xOnQuad / targetWidth;
        float localY = 1.0f - (yOnQuad / targetHeight);

        localX = 1.0f - localX;

        localX = Clamp(localX, 0.0f, 1.0f);
        localY = Clamp(localY, 0.0f, 1.0f);
        if(this->limbs.torso.is_intact)
        { 
            if (this->limbs.head.is_intact && localY < 0.20f)
            {
                return BodyPart::HEAD;
            }
            else if (localY < 0.60f)
            {
                if (this->limbs.left_arm.is_intact && localX < 0.30f)
                {
                    return BodyPart::LEFT_ARM;
                }
                else if (this->limbs.right_arm.is_intact && localX > 0.70f)
                {
                    return BodyPart::RIGHT_ARM;
                }
                else
                {
                    return BodyPart::TORSO;
                }
            }
            else
            {
                if (this->limbs.left_leg.is_intact && localX < 0.50f)
                {
                    return BodyPart::LEFT_LEG;
                }

                else if (this->limbs.right_leg.is_intact && localX > 0.50f)
                {
                    return BodyPart::RIGHT_LEG;
                }
            }
        }
        
    }

    return BodyPart::NONE;
}

BodyPart pimpek::calculate_hovered_body_part(Vector3 drawPos, float targetWidth, float targetHeight, Camera3D camera)
{
    Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);

    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
    Vector3 up = Vector3Normalize(camera.up);

    float halfW = targetWidth * 0.5f;
    float halfH = targetHeight * 0.5f;

    Vector3 topLeft = Vector3Add(Vector3Subtract(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
    Vector3 topRight = Vector3Add(Vector3Add(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
    Vector3 bottomRight = Vector3Subtract(Vector3Add(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
    Vector3 bottomLeft = Vector3Subtract(Vector3Subtract(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));

    RayCollision collision = GetRayCollisionQuad(ray, topLeft, topRight, bottomRight, bottomLeft);

    if (collision.hit)
    {
        Vector3 bottomLeft = Vector3Subtract(Vector3Subtract(drawPos, Vector3Scale(right, halfW)), Vector3Scale(up, halfH));
        Vector3 rel = Vector3Subtract(collision.point, bottomLeft);

        float xOnQuad = Vector3DotProduct(rel, right);
        float yOnQuad = Vector3DotProduct(rel, up);

        float localX = xOnQuad / targetWidth;
        float localY = 1.0f - (yOnQuad / targetHeight);

        localX = 1.0f - localX;

        localX = Clamp(localX, 0.0f, 1.0f);
        localY = Clamp(localY, 0.0f, 1.0f);

        if (localX < 0.28f || localX > 0.72f || localY < 0.22f || localY > 0.74f)
        {
            return BodyPart::NONE;
        }

        if (this->limbs.torso.is_intact)
        {
            // 1. NOGI (dół postaci: Y od 0.53 do 0.74)
            if (localY >= 0.53f)
            {
                if (localX > 0.50f)
                {
                    if (this->limbs.right_leg.is_intact) return BodyPart::RIGHT_LEG;
                    if (this->limbs.left_leg.is_intact)  return BodyPart::LEFT_LEG;
                }
                else
                {
                    if (this->limbs.left_leg.is_intact)  return BodyPart::LEFT_LEG;
                    if (this->limbs.right_leg.is_intact) return BodyPart::RIGHT_LEG;
                }
                return BodyPart::TORSO;
            }

            bool arms_intact = (this->limbs.left_arm.is_intact || this->limbs.right_arm.is_intact);
            if (arms_intact && localY >= 0.46f && localY < 0.53f && localX >= 0.38f && localX <= 0.62f)
            {
                return BodyPart::LEFT_ARM; // Reprezentuje obie połączone łapki
            }
            if (this->limbs.head.is_intact && localY < 0.46f)
            {
                return BodyPart::HEAD;
            }

            if (!this->limbs.head.is_intact && localY < 0.38f)
            {
                return BodyPart::NONE;
            }

            return BodyPart::TORSO;
        }
    }

    return BodyPart::NONE;
}

int pimpek::take_damage(int dmg_amount, const character* attacker, bool is_crit, bool is_guard, BodyPart hit_part, gamestate* gs, Vector3 impact_pos)
{
    if (hit_part == BodyPart::LEFT_ARM || hit_part == BodyPart::RIGHT_ARM)
    {
        int dmg = character::take_damage(dmg_amount, attacker, is_crit, is_guard, BodyPart::LEFT_ARM, gs, impact_pos);
        this->limbs.right_arm.hp = this->limbs.left_arm.hp;
        this->limbs.right_arm.is_intact = this->limbs.left_arm.is_intact;
        return dmg;
    }

    return character::take_damage(dmg_amount, attacker, is_crit, is_guard, hit_part, gs, impact_pos);
}

BodyPart enemy::select_random_target_part(const character& target)
{
    std::vector<BodyPart> valid_parts;

    if (target.limbs.torso.is_intact)     valid_parts.push_back(BodyPart::TORSO);
    if (target.limbs.head.is_intact)      valid_parts.push_back(BodyPart::HEAD);
    if (target.limbs.left_arm.is_intact)  valid_parts.push_back(BodyPart::LEFT_ARM);
    if (target.limbs.right_arm.is_intact) valid_parts.push_back(BodyPart::RIGHT_ARM);
    if (target.limbs.left_leg.is_intact)  valid_parts.push_back(BodyPart::LEFT_LEG);
    if (target.limbs.right_leg.is_intact) valid_parts.push_back(BodyPart::RIGHT_LEG);

    if (valid_parts.empty())
    {
        return BodyPart::TORSO;
    }

    int randomIndex = GetRandomValue(0, (int)valid_parts.size() - 1);
    return valid_parts[randomIndex];
}

/*int enemy::execute_ai_turn(character& target)
{
    if (!global_fx.is_playing && target.queued_damage > 0)
    {
        this->take_damage(target.queued_damage, &target, false, false, target.queued_hit_part, this->gs);
        target.queued_damage = 0.0;
		target.queued_hit_part = BodyPart::NONE;
    }

    auto [e_dmg, crit] = this->calculate_dmg(this->get_damage());
    audio.play_sound(SoundID::ENEMY_HIT);
    int final_dmg = target.take_damage(e_dmg, this, crit, target.is_guard, target.queued_hit_part, this->gs);

    target.is_guard = false;

    return final_dmg;
}*/


int ghoul::execute_ai_turn(character& target)
{
    if (!global_fx.is_playing && target.queued_damage > 0)
    {
        this->take_damage((int)target.queued_damage, &target, false, false, target.queued_hit_part, this->gs);
        target.queued_damage = 0.0f;
        target.queued_hit_part = BodyPart::NONE;
    }
    if (this->is_dead()) return 0;

    this->process_turn_start_effects(this->gs);
    if (this->is_dead()) return 0;

    int ghoul_output_dmg = 0;
    this->attack_part_1 = BodyPart::NONE;

    const auto& right_arm = this->limbs.get_limb(BodyPart::RIGHT_ARM);
    const auto& head = this->limbs.get_limb(BodyPart::HEAD);

    if (right_arm.is_intact && right_arm.can_attack)
    {
        this->attack_part_1 = BodyPart::RIGHT_ARM;
        ghoul_output_dmg = right_arm.damage;
    }
    else if (head.is_intact && head.can_attack)
    {
        this->attack_part_1 = BodyPart::HEAD;
        ghoul_output_dmg = head.damage;
    }

    if (this->attack_part_1 != BodyPart::NONE)
    {
        const auto& attacking_limb = this->limbs.get_limb(this->attack_part_1);
        if (attacking_limb.applies_bleed)
        {
            if (!target.is_bleeding)
            {
                gamestate::gameLogs.push_back(TextFormat("You started bleeding!"));
            }

            target.is_bleeding = true;
            target.applied_bleed_damage += attacking_limb.bleed_dmg;
            target.bleed_status_timer = attacking_limb.bleed_timer;
        }
    }

    BodyPart hit_part = this->select_random_target_part(target);
    auto [e_dmg, crit] = this->calculate_dmg(ghoul_output_dmg);
    audio.play_sound(SoundID::ENEMY_HIT);

    int final_dmg = target.take_damage(e_dmg, this, crit, target.is_guard, hit_part, this->gs);
    target.is_guard = false;

    return final_dmg;
}
int pimpek::execute_ai_turn(character& target)
{
    if (!global_fx.is_playing && target.queued_damage > 0)
    {
        this->take_damage((int)target.queued_damage, &target, false, false, target.queued_hit_part, this->gs);
        target.queued_damage = 0.0f;
        target.queued_hit_part = BodyPart::NONE;
    }
    if (this->is_dead()) return 0;

    this->process_turn_start_effects(this->gs);
    if (this->is_dead()) return 0;

    this->attack_part_1 = BodyPart::NONE;
    this->attack_part_2 = BodyPart::NONE;

    const auto& right_leg = this->limbs.get_limb(BodyPart::RIGHT_LEG);
    const auto& left_leg = this->limbs.get_limb(BodyPart::LEFT_LEG);

    if (right_leg.is_intact && right_leg.can_attack)
    {
        this->attack_part_1 = BodyPart::RIGHT_LEG;
    }
    if (left_leg.is_intact && left_leg.can_attack)
    {
        this->attack_part_2 = BodyPart::LEFT_LEG;
    }

    int total_dmg = 0;
    bool was_guarding = target.is_guard;

    auto perform_strike = [&](BodyPart attacking_part) {
        if (target.is_dead()) return;

        const auto& attacking_limb = this->limbs.get_limb(attacking_part);
        if (attacking_limb.applies_bleed)
        {
            if (!target.is_bleeding)
            {
                gamestate::gameLogs.push_back(TextFormat("You started bleeding!"));
            }

            target.is_bleeding = true;
            target.applied_bleed_damage += attacking_limb.bleed_dmg;
            target.bleed_status_timer = attacking_limb.bleed_timer;
        }

        BodyPart hit_part = this->select_random_target_part(target);
        auto [e_dmg, crit] = this->calculate_dmg(attacking_limb.damage);
        audio.play_sound(SoundID::ENEMY_HIT);

        int dmg = target.take_damage(e_dmg, this, crit, was_guarding, hit_part, this->gs);
        total_dmg += dmg;
    };

    if (this->attack_part_1 != BodyPart::NONE)
    {
        perform_strike(this->attack_part_1);
    }

    if (this->attack_part_2 != BodyPart::NONE)
    {
        perform_strike(this->attack_part_2);
    }

    target.is_guard = false;

    return total_dmg;
}




int enemy::GetAdjustedSideLimit(int baseSideLimit, int frameIndex)
{
    if (baseSideLimit == 0)
    {
        return 0;
    }

    bool isBackView = (frameIndex >= 2 && frameIndex <= 6); 
    if (isBackView)
    {
        return (baseSideLimit == 1) ? 2 : 1;
    }

    return baseSideLimit;
}
   

