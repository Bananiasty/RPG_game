#include "character.h"
#include "raylib.h"
#include "raymath.h"
#include "gamestates.h"
#include "struct.h"



void exploration::update_enemies()
{
    float dt = GetFrameTime();
    float detectionRange = 8.0f;
    float enemySpeed = 2.0f;

    for (auto& [id, node] : world_map)
    {
        enemy* e = node.enemy;

        if (!e || e->is_dead())
        {
            continue;
        }

        Vector3 enemyPos = e->get_position();
        float distance = Vector3Distance(enemyPos, bohater.position);

        if (distance <= detectionRange)
        {

            if (!e->is_moving())
            {
                apply_pathfinding(e);
            }

            if (e->is_moving())
            {

                Vector3 target = e->get_target_tile();
                Vector3 nextPos = Vector3MoveTowards(enemyPos, target, enemySpeed * dt);

                Vector3 stary_cam_pos = camera.position;
                Vector3 stary_cam_target = camera.target;

                camera.position = nextPos;
                camera.target = nextPos;

                apply_collision(enemyPos);

                Vector3 finalPos = camera.position;
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

BodyPart enemy::calculate_hovered_body_part(Vector3 drawPos, float targetWidth, float targetHeight, Camera3D camera)
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
        float localX = (collision.point.x - (drawPos.x - targetWidth * 0.5f)) / targetWidth;
        float localY = 1.0f - ((collision.point.y - (drawPos.y - targetHeight * 0.5f)) / targetHeight);

        localX = Clamp(localX, 0.0f, 1.0f);
        localY = Clamp(localY, 0.0f, 1.0f);

        if (localY < 0.20f)
        {
            return BodyPart::HEAD;
        }
        else if (localY < 0.60f)
        {
            if (localX < 0.30f)      return BodyPart::LEFT_ARM;
            else if (localX > 0.70f) return BodyPart::RIGHT_ARM;
            else                     return BodyPart::TORSO;
        }
        else
        {
            if (localX < 0.50f)      return BodyPart::LEFT_LEG;
            else                     return BodyPart::RIGHT_LEG;
        }
    }

    return BodyPart::NONE;
}

int enemy::execute_ai_turn(character& target)
{
    if (!global_fx.is_playing && target.queued_damage > 0)
    {
        this->take_damage(target.queued_damage, &target, false, false);
        target.queued_damage = 0.0;
    }

    auto [e_dmg, crit] = this->calculate_dmg();
    int final_dmg = target.take_damage(e_dmg, this, crit, target.is_guard);

    target.is_guard = false;

    return final_dmg;
}

   

