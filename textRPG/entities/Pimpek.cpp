#include "entities/Pimpek.h"
#include "gamestates.h"
#include "AudioManager.h"
#include "raymath.h"

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

std::vector<BodyPart> pimpek::decide_attacks()
{
    std::vector<BodyPart> attacks;
    const auto& right_leg = this->limbs.get_limb(BodyPart::RIGHT_LEG);
    const auto& left_leg = this->limbs.get_limb(BodyPart::LEFT_LEG);

    if (right_leg.is_intact && right_leg.can_attack)
    {
        attacks.push_back(BodyPart::RIGHT_LEG);
    }
    if (left_leg.is_intact && left_leg.can_attack)
    {
        attacks.push_back(BodyPart::LEFT_LEG);
    }

    return attacks;
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

    auto attacks = decide_attacks();
    int total_dmg = 0;
    for (BodyPart part : attacks)
    {
        total_dmg += perform_strike(part, target);
    }
    target.is_guard = false;

    return total_dmg;
}
