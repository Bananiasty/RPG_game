#include "struct.h"
#include "gamestates.h"
#include "raylib.h"
#include "raymath.h"
#include "character.h"




collisions check_collisions(Vector3 pos, exploration* exp)
{
    collisions c;

    const auto& current_dungeon = exp->floors[exp->current_floor_id].dungeon;
    float p_hitbox = 0.25f;

    int idx_plus_x = static_cast<int>((pos.x + p_hitbox) / 2.0f);
    int idx_minus_x = static_cast<int>((pos.x - p_hitbox) / 2.0f);
    int idx_z = static_cast<int>(pos.z / 2.0f);

    int idx_x = static_cast<int>(pos.x / 2.0f);
    int idx_plus_z = static_cast<int>((pos.z + p_hitbox) / 2.0f);
    int idx_minus_z = static_cast<int>((pos.z - p_hitbox) / 2.0f);

    if (idx_plus_x >= 0 && idx_plus_x < exp->szerokosc && idx_z >= 0 && idx_z < exp->dlugosc)
    {
        if (current_dungeon[idx_plus_x][idx_z] != 1) c.blokuj_ruch_plus_x = true;
    }

    if (idx_minus_x >= 0 && idx_minus_x < exp->szerokosc && idx_z >= 0 && idx_z < exp->dlugosc)
    {
        if (current_dungeon[idx_minus_x][idx_z] != 1) c.blokuj_ruch_minus_x = true;
    }

    if (idx_x >= 0 && idx_x < exp->szerokosc && idx_plus_z >= 0 && idx_plus_z < exp->dlugosc)
    {
        if (current_dungeon[idx_x][idx_plus_z] != 1) c.blokuj_ruch_plus_z = true;
    }

    if (idx_x >= 0 && idx_x < exp->szerokosc && idx_minus_z >= 0 && idx_minus_z < exp->dlugosc)
    {
        if (current_dungeon[idx_x][idx_minus_z] != 1) c.blokuj_ruch_minus_z = true;
    }

    return c;
}

void exploration::apply_collision(Vector3 stara_pos)
{
    if (floors.empty() || current_floor_id < 0 || current_floor_id >= static_cast<int>(floors.size()))
    {
        return;
    }

    const auto& current_dungeon = floors[current_floor_id].dungeon;
    float p_hitbox = 0.25f;

    int check_x = static_cast<int>((camera.position.x + (camera.position.x > stara_pos.x ? p_hitbox : -p_hitbox)) / 2.0f) + 1;
    int current_z = static_cast<int>(camera.position.z / 2.0f);

    if (check_x >= 0 && check_x < szerokosc && current_z >= 0 && current_z < dlugosc)
    {
        if (current_dungeon[check_x][current_z] != 1)
        {
            float delta_x = camera.position.x - stara_pos.x;
            camera.position.x = stara_pos.x;
            camera.target.x -= delta_x;
        }
    }

    int current_x = static_cast<int>(camera.position.x / 2.0f) + 1;
    int check_z = static_cast<int>((camera.position.z + (camera.position.z > stara_pos.z ? p_hitbox : -p_hitbox)) / 2.0f);

    if (current_x >= 0 && current_x < szerokosc && check_z >= 0 && check_z < dlugosc)
    {
        if (current_dungeon[current_x][check_z] != 1)
        {
            float delta_z = camera.position.z - stara_pos.z;
            camera.position.z = stara_pos.z;
            camera.target.z -= delta_z;
        }
    }
}

bool exploration::is_walkable(int x, int y)
{
    if (x < 0 || x >= szerokosc || y < 0 || y >= dlugosc)
    {
        return false;
    }

    if (floors.empty() || current_floor_id < 0 || current_floor_id >= static_cast<int>(floors.size()))
    {
        return false;
    }

    return floors[current_floor_id].dungeon[x][y] == 1;
}
bool exploration::is_walkable_subgrid(int gridX, int gridZ)
{
    int dungeonX = (gridX >= 0) ? (gridX / 4) : ((gridX - 3) / 4);
    int dungeonZ = (gridZ >= 0) ? (gridZ / 4) : ((gridZ - 3) / 4);

    return is_walkable(dungeonX, dungeonZ);
}

bool exploration::is_near_wall(int gridX, int gridZ)
{
    const GridPosition checks[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

    for (int i = 0; i < 4; i++)
    {
        int nGridX = gridX + checks[i].x;
        int nGridZ = gridZ + checks[i].z;

        int nDungeonX = (nGridX >= 0) ? (nGridX / 4) : ((nGridX - 3) / 4);
        int nDungeonZ = (nGridZ >= 0) ? (nGridZ / 4) : ((nGridZ - 3) / 4);

        if (!is_walkable(nDungeonX, nDungeonZ))
        {
            return true;
        }
    }
    return false;
}

static int CalculateHeuristic(int startX, int startZ, int targetX, int targetZ)
{
	int dx = std::abs(startX - targetX);
	int dz = std::abs(startZ - targetZ);
	int diagonal = std::min(dx, dz);
	int straight = std::abs(dx - dz);
	return 14 * diagonal + 10 * straight;
}
static int find_in_list(const std::vector<tile_path>& list, int x, int z)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].x == x && list[i].z == z)
			return i;
	}
	return -1;
}

static bool is_in_list(const std::vector<tile_path>& list, int x, int z)
{
	return find_in_list(list, x, z) != -1;
}

std::vector<GridPosition> exploration::find_path(int startX, int startZ, int targetX, int targetZ)
{
    std::vector<GridPosition> path;

    if (!is_walkable_subgrid(targetX, targetZ) || (startX == targetX && startZ == targetZ))
    {
        return path;
    }

    std::vector<tile_path> openList;
    std::vector<tile_path> closedList;

    tile_path startNode;
    startNode.x = startX;
    startNode.z = startZ;
    startNode.gCost = 0;
    startNode.hCost = CalculateHeuristic(startX, startZ, targetX, targetZ);
    startNode.parent = { startX, startZ };

    openList.push_back(startNode);

    const GridPosition directions[8] = {
        { 0, -1}, { 0,  1}, {-1,  0}, { 1,  0}, // Góra, Dó³, Lewo, Prawo
        {-1, -1}, { 1, -1}, {-1,  1}, { 1,  1}  // Skosy
    };

    while (!openList.empty())
    {
        int bestIndex = 0;
        for (size_t i = 1; i < openList.size(); i++)
        {
            if (openList[i].fCost() < openList[bestIndex].fCost())
            {
                bestIndex = i;
            }
        }

        tile_path current = openList[bestIndex];

        if (current.x == targetX && current.z == targetZ)
        {
            tile_path temp = current;
            while (!(temp.x == startX && temp.z == startZ))
            {
                path.push_back({ temp.x, temp.z });

                if (temp.parent.x == startX && temp.parent.z == startZ)
                {
                    break;
                }

                int parentIndex = find_in_list(closedList, temp.parent.x, temp.parent.z);
                if (parentIndex != -1)
                {
                    temp = closedList[parentIndex];
                }
                else
                {
                    break;
                }
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

        openList.erase(openList.begin() + bestIndex);
        closedList.push_back(current);

        for (int i = 0; i < 8; i++)
        {
            int nX = current.x + directions[i].x;
            int nZ = current.z + directions[i].z;

            if (!is_walkable_subgrid(nX, nZ))
            {
                continue;
            }

            if (i >= 4)
            {
                if (!is_walkable_subgrid(current.x + directions[i].x, current.z) &&
                    !is_walkable_subgrid(current.x, current.z + directions[i].z))
                {
                    continue;
                }
            }

            if (is_in_list(closedList, nX, nZ))
            {
                continue;
            }

            int moveCost = (i < 4) ? 10 : 14;
            if (is_near_wall(nX, nZ))
            {
                moveCost += 50; 
            }
            int newGCost = current.gCost + moveCost;

            int openIndex = find_in_list(openList, nX, nZ);

            if (openIndex == -1)
            {
                tile_path neighbor;
                neighbor.x = nX;
                neighbor.z = nZ;
                neighbor.gCost = newGCost;
                neighbor.hCost = CalculateHeuristic(nX, nZ, targetX, targetZ);
                neighbor.parent = { current.x, current.z };

                openList.push_back(neighbor);
            }
            else if (newGCost < openList[openIndex].gCost)
            {
                openList[openIndex].gCost = newGCost;
                openList[openIndex].parent = { current.x, current.z };
            }
        }
    }

    return path;
}

void exploration::apply_pathfinding(enemy* e)
{
    if (e == nullptr) return;

    float nodeSize = 0.5f;
    Vector3 enemyPos = e->get_position();
    Vector3 playerPos = this->bohater.get_position();

    float distanceToPlayer = Vector3Distance(enemyPos, playerPos);
    if (distanceToPlayer <= 0.6f)
    {
        Vector3 moveDir = Vector3Normalize(Vector3Subtract(playerPos, enemyPos));
        if (moveDir.x != 0.0f || moveDir.z != 0.0f)
        {
            e->set_forward(moveDir);
        }
        e->set_target_tile(playerPos);
        e->set_is_moving(true);
        return;
    }

    int e_grid_x = static_cast<int>(std::floor(enemyPos.x / nodeSize)) + 4;
    int e_grid_z = static_cast<int>(std::floor(enemyPos.z / nodeSize));

    int player_grid_x = static_cast<int>(std::floor(playerPos.x / nodeSize)) + 4;
    int player_grid_z = static_cast<int>(std::floor(playerPos.z / nodeSize));

    std::vector<GridPosition> path = find_path(e_grid_x, e_grid_z, player_grid_x, player_grid_z);

    if (!path.empty())
    {
        Vector3 nextTileWorld;

        if (path.size() == 1)
        {
            nextTileWorld = playerPos;
        }
        else
        {
            GridPosition nextStep = path[0];

            nextTileWorld = {
                (nextStep.x - 4) * nodeSize + (nodeSize / 2.0f),
                enemyPos.y,
                nextStep.z * nodeSize + (nodeSize / 2.0f)
            };
        }

        Vector3 moveDir = Vector3Normalize(Vector3Subtract(nextTileWorld, enemyPos));

        if (moveDir.x != 0.0f || moveDir.z != 0.0f)
        {
            e->set_forward(moveDir);
        }

        e->set_target_tile(nextTileWorld);
        e->set_is_moving(true);
    }
}