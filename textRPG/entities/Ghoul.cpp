#include "entities/Ghoul.h"
#include "gamestates.h"
#include "AudioManager.h"
#include "raymath.h"

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
        Vector3 rel = Vector3Subtract(collision.point, bottomLeft);

        float xOnQuad = Vector3DotProduct(rel, right);
        float yOnQuad = Vector3DotProduct(rel, up);

        float localX = xOnQuad / targetWidth;
        float localY = 1.0f - (yOnQuad / targetHeight);

        localX = 1.0f - localX;

        localX = Clamp(localX, 0.0f, 1.0f);
        localY = Clamp(localY, 0.0f, 1.0f);
        if (this->limbs.torso.is_intact)
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
