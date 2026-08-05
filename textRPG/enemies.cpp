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

BodyPart exploration::GetHoveredBodyPart(Vector3 drawPos, float targetWidth, float targetHeight, Camera3D camera)
{
    Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);

    BoundingBox enemyBox = {
        { drawPos.x - targetWidth * 0.5f, drawPos.y - targetHeight * 0.5f, drawPos.z - 0.2f },
        { drawPos.x + targetWidth * 0.5f, drawPos.y + targetHeight * 0.5f, drawPos.z + 0.2f }
    };

    RayCollision collision = GetRayCollisionBox(ray, enemyBox);

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

SpriteRenderParams GetGhoulRenderParams(Vector3 entityPos, Texture2D texture)
{
    constexpr float GHOUL_Y_OFFSET = 0.15f;

    float fWidth = (float)texture.width / 8.0f;
    float fHeight = (float)texture.height / 4.0f;
    float tHeight = 2.3f;
    float tWidth = tHeight * (fWidth / fHeight);

    Vector3 drawPos = { entityPos.x, entityPos.y + GHOUL_Y_OFFSET, entityPos.z };

    return { drawPos, tWidth, tHeight, fWidth, fHeight };
}