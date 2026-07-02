#include "struct.h"
#include "gamestates.h"
#include "raylib.h"




collisions check_collisions(Vector3 pos, exploration* exp)
{
	collisions c;

	float p_hitbox = 0.25;

	if (exp->dungeon[(pos.x + p_hitbox)/2][pos.z/2] != 1)
	{
		c.blokuj_ruch_plus_x = true;
	}
	if (exp->dungeon[(pos.x - p_hitbox) / 2][pos.z / 2] != 1)
	{
		c.blokuj_ruch_minus_x = true;
	}
	if (exp->dungeon[(pos.x) / 2][(pos.z + p_hitbox) / 2] != 1)
	{
		c.blokuj_ruch_plus_z = true;
	}
	if (exp->dungeon[(pos.x) / 2][(pos.z - p_hitbox) / 2] != 1)
	{
		c.blokuj_ruch_minus_z = true;
	}

	
	return c;
}

void exploration::apply_collision(Vector3 stara_pos)
{
	check_collisions(camera.position, this);
	float p_hitbox = 0.25;

	int check_x = (int)((camera.position.x + (camera.position.x > stara_pos.x ? p_hitbox : -p_hitbox)) / 2.0f)+1;
	int current_z = (int)(camera.position.z / 2.0f);

	if (check_x >= 0 && check_x < szerokosc && current_z >= 0 && current_z < dlugosc)
	{
		if (dungeon[check_x][current_z] != 1)
		{
			float delta_x = camera.position.x - stara_pos.x;

			camera.position.x = stara_pos.x;

			camera.target.x -= delta_x;
		}

	}

	int current_x = (int)(camera.position.x / 2.0f)+1;
	int check_z = (int)((camera.position.z + (camera.position.z > stara_pos.z ? p_hitbox : -p_hitbox)) / 2.0f);

	if (current_x >= 0 && current_x < szerokosc && check_z >= 0 && check_z < dlugosc)
	{
		if (dungeon[current_x][check_z] != 1)
		{
			float delta_z = camera.position.z - stara_pos.z;

			camera.position.z = stara_pos.z;

			camera.target.z -= delta_z;
		}
	}

}