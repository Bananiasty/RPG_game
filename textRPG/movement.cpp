#include "struct.h"
#include "gamestates.h"
#include "raylib.h"
#include "raymath.h"
#include "entities/Player.h"
#include "entities/Enemy.h"




collisions check_collisions(Vector3 pos, exploration* exp)
{
    collisions c;

    if (exp == nullptr)
    {
        return c;
    }

    float p_hitbox = 0.25f;

    int idx_plus_x = static_cast<int>((pos.x + p_hitbox) / 2.0f) + 1;
    int idx_minus_x = static_cast<int>((pos.x - p_hitbox) / 2.0f) + 1;
    int idx_x = static_cast<int>(pos.x / 2.0f) + 1;

    int idx_z = static_cast<int>(pos.z / 2.0f);
    int idx_plus_z = static_cast<int>((pos.z + p_hitbox) / 2.0f);
    int idx_minus_z = static_cast<int>((pos.z - p_hitbox) / 2.0f);

    if (!exp->is_walkable(idx_plus_x, idx_z))   c.blokuj_ruch_plus_x = true;
    if (!exp->is_walkable(idx_minus_x, idx_z))  c.blokuj_ruch_minus_x = true;
    if (!exp->is_walkable(idx_x, idx_plus_z))   c.blokuj_ruch_plus_z = true;
    if (!exp->is_walkable(idx_x, idx_minus_z))  c.blokuj_ruch_minus_z = true;

    return c;
}

void exploration::apply_collision(Vector3 stara_pos)
{
    if (floors.empty() || current_floor_id < 0 || current_floor_id >= static_cast<int>(floors.size()))
    {
        return;
    }

    float p_hitbox = 0.25f;

    // Test wzdĹ‚uĹĽ osi X
    int check_x = static_cast<int>((camera.position.x + (camera.position.x > stara_pos.x ? p_hitbox : -p_hitbox)) / 2.0f) + 1;
    int current_z = static_cast<int>(camera.position.z / 2.0f);

    if (!is_walkable(check_x, current_z))
    {
        float delta_x = camera.position.x - stara_pos.x;
        camera.position.x = stara_pos.x;
        camera.target.x -= delta_x;
    }

    // Test wzdĹ‚uĹĽ osi Z
    int current_x = static_cast<int>(camera.position.x / 2.0f) + 1;
    int check_z = static_cast<int>((camera.position.z + (camera.position.z > stara_pos.z ? p_hitbox : -p_hitbox)) / 2.0f);

    if (!is_walkable(current_x, check_z))
    {
        float delta_z = camera.position.z - stara_pos.z;
        camera.position.z = stara_pos.z;
        camera.target.z -= delta_z;
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

    const auto& cur_floor = floors[current_floor_id];

    if (cur_floor.dungeon[x][y] != 1)
    {
        return false;
    }

    if (cur_floor.is_tile_solid(x, y))
    {
        return false;
    }

    return true;
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

bool exploration::has_line_of_sight(Vector3 start, Vector3 end)
{
    float dist = Vector3Distance(start, end);
    if (dist < 0.05f) return true;

    float stepSize = 0.20f;
    int steps = static_cast<int>(std::ceil(dist / stepSize));
    if (steps < 2) steps = 2;

    Vector3 dir = Vector3Scale(Vector3Subtract(end, start), 1.0f / (float)steps);

    Vector3 current = start;
    for (int i = 0; i <= steps; ++i)
    {
        int dungeonX = static_cast<int>(std::floor(current.x / 2.0f)) + 1;
        int dungeonZ = static_cast<int>(std::floor(current.z / 2.0f));

        if (!is_walkable(dungeonX, dungeonZ))
        {
            return false;
        }

        current = Vector3Add(current, dir);
    }

    return true;
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

std::vector<GridPosition> exploration::find_path(int startX, int startZ, int targetX, int targetZ, enemy* current_enemy, bool ignore_other_enemies)
{
    std::vector<GridPosition> path;

    if (startX == targetX && startZ == targetZ)
    {
        return path;
    }

    // Jesli docelowy kafelek gracza nie jest osiagalny (np. gracz stoi przy scianie/obiekcie),
    // szukamy najblizszego chodzalnego kafelka w otoczeniu gracza
    if (!is_walkable_subgrid(targetX, targetZ))
    {
        int bestDistSq = 999999;
        int bestX = targetX;
        int bestZ = targetZ;
        bool foundWalkable = false;

        for (int r = 1; r <= 4; ++r)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                for (int dz = -r; dz <= r; ++dz)
                {
                    int tx = targetX + dx;
                    int tz = targetZ + dz;
                    if (is_walkable_subgrid(tx, tz))
                    {
                        int distSq = dx * dx + dz * dz;
                        if (distSq < bestDistSq)
                        {
                            bestDistSq = distSq;
                            bestX = tx;
                            bestZ = tz;
                            foundWalkable = true;
                        }
                    }
                }
            }
            if (foundWalkable) break;
        }

        if (foundWalkable)
        {
            targetX = bestX;
            targetZ = bestZ;
        }
        else
        {
            return path;
        }
    }

    if (startX == targetX && startZ == targetZ)
    {
        return path;
    }

    struct OccupiedEnemy
    {
        int gx;
        int gz;
        int tx;
        int tz;
        bool has_target;
    };
    std::vector<OccupiedEnemy> other_enemies;

    float nodeSize = 0.5f;
    if (!ignore_other_enemies && current_floor_id >= 0 && current_floor_id < static_cast<int>(floors.size()))
    {
        for (enemy* other : floors[current_floor_id].active_enemies)
        {
            if (other != nullptr && !other->is_dead() && other != current_enemy)
            {
                Vector3 opos = other->get_position();
                int ogx = static_cast<int>(std::floor(opos.x / nodeSize)) + 4;
                int ogz = static_cast<int>(std::floor(opos.z / nodeSize));

                int otx = ogx;
                int otz = ogz;
                bool has_tgt = false;
                if (other->is_moving())
                {
                    Vector3 otgt = other->get_target_tile();
                    otx = static_cast<int>(std::floor(otgt.x / nodeSize)) + 4;
                    otz = static_cast<int>(std::floor(otgt.z / nodeSize));
                    has_tgt = true;
                }
                other_enemies.push_back({ ogx, ogz, otx, otz, has_tgt });
            }
        }
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

    tile_path bestNode = startNode;
    int bestH = startNode.hCost;

    const GridPosition directions[8] = {
        { 0, -1}, { 0,  1}, {-1,  0}, { 1,  0}, // Góra, Dół, Lewo, Prawo
        {-1, -1}, { 1, -1}, {-1,  1}, { 1,  1}  // Skosy
    };

    while (!openList.empty())
    {
        size_t bestIndex = 0;
        for (size_t i = 1; i < openList.size(); i++)
        {
            if (openList[i].fCost() < openList[bestIndex].fCost() ||
               (openList[i].fCost() == openList[bestIndex].fCost() && openList[i].hCost < openList[bestIndex].hCost))
            {
                bestIndex = i;
            }
        }

        tile_path current = openList[bestIndex];

        if (current.hCost < bestH)
        {
            bestH = current.hCost;
            bestNode = current;
        }

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

            // Zapobieganie scinaniu rogow scian przez ruch po skosie:
            // Obaj prostopadli sasiedzi musza byc wolni
            if (i >= 4)
            {
                if (!is_walkable_subgrid(current.x + directions[i].x, current.z) ||
                    !is_walkable_subgrid(current.x, current.z + directions[i].z))
                {
                    continue;
                }
            }

            if (is_in_list(closedList, nX, nZ))
            {
                continue;
            }

            bool is_blocked_by_enemy = false;
            int enemy_proximity_cost = 0;

            for (const auto& oe : other_enemies)
            {
                // Nigdy nie blokujemy docelowej pozycji gracza
                if (nX == targetX && nZ == targetZ)
                {
                    break;
                }

                // Bezposredni kafelek, na ktorym stoi inny wrog
                if ((nX == oe.gx && nZ == oe.gz) || (oe.has_target && nX == oe.tx && nZ == oe.tz))
                {
                    is_blocked_by_enemy = true;
                    break;
                }

                // Strefa wokol innego wroga - duzy koszt, by omijac go z dystansu
                int dist_cur = std::max(std::abs(nX - oe.gx), std::abs(nZ - oe.gz));
                if (dist_cur <= 1)
                {
                    enemy_proximity_cost = std::max(enemy_proximity_cost, 80);
                }
                if (oe.has_target)
                {
                    int dist_tgt = std::max(std::abs(nX - oe.tx), std::abs(nZ - oe.tz));
                    if (dist_tgt <= 1)
                    {
                        enemy_proximity_cost = std::max(enemy_proximity_cost, 80);
                    }
                }
            }

            if (is_blocked_by_enemy)
            {
                continue;
            }

            int moveCost = (i < 4) ? 10 : 14;
            if (is_near_wall(nX, nZ))
            {
                moveCost += 20; 
            }
            moveCost += enemy_proximity_cost;
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

    // Jesli nie udalo sie dotrzec bezposrednio do celu, zwracamy najblizsza mozliwa sciezke prowadzaca w strone gracza
    if (bestNode.x != startX || bestNode.z != startZ)
    {
        tile_path temp = bestNode;
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

    return path;
}

void exploration::apply_pathfinding(enemy* e)
{
    if (e == nullptr) return;

    float nodeSize = 0.5f;
    Vector3 enemyPos = e->get_position();
    Vector3 playerPos = this->bohater.get_position();

    float distanceToPlayer = Vector3Distance(enemyPos, playerPos);
    // Bezposredni krok do gracza tylko wtedy, gdy nie ma miedzy nimi sciany (jest linia wzroku)
    if (distanceToPlayer <= 0.6f && has_line_of_sight(enemyPos, playerPos))
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

    std::vector<GridPosition> path = find_path(e_grid_x, e_grid_z, player_grid_x, player_grid_z, e);

    if (!path.empty())
    {
        Vector3 nextTileWorld;

        // Do gracza idziemy bezposrednio tylko wtedy, gdy ostatni krok ma wolna linie wzroku
        if (path.size() == 1 && has_line_of_sight(enemyPos, playerPos))
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