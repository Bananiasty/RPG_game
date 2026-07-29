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
    float stepSize = 0.5f;

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
                float dx = bohater.position.x - enemyPos.x;
                float dz = bohater.position.z - enemyPos.z;

                float stepX = 0.0f;

                if (std::abs(dx) > 0.5f)
                {
                    if (dx > 0)
                    {
                        stepX = 1.0f; 
                    }
                    else
                    {
                        stepX = -1.0f;
                    }
                }
                else
                {
                    stepX = 0.0f; 
                }
                float stepZ = 0.0f;

                if (std::abs(dz) > 0.5f)
                {
                    if (dz > 0)
                    {
                        stepZ = 1.0f; 
                    }
                    else
                    {
                        stepZ = -1.0f;
                    }
                }
                else
                {
                    stepZ = 0.0f;  
                }


                if (stepX != 0.0f || stepZ != 0.0f)
                {
                    Vector3 nextTile = {
                        enemyPos.x + stepX * stepSize,
                        enemyPos.y,
                        enemyPos.z + stepZ * stepSize
                    };

                    if (!e->is_moving())
                    {
                        if (stepX != 0.0f || stepZ != 0.0f)
                        {
                            Vector3 moveDir = Vector3Normalize({ stepX, 0.0f, stepZ });
                            e->set_forward(moveDir);

                            e->set_target_tile(nextTile);
                            e->set_is_moving(true);
                        }
                    }
                }
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

                if (Vector3Distance(finalPos, target) < 0.05f)
                {
                    e->set_position(target);
                    e->set_is_moving(false);
                }
            }
        }
    }
}