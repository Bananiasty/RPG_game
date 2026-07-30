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