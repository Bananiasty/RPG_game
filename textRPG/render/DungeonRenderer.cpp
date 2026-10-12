#include "render/DungeonRenderer.h"
#include "ui/HUD.h"
#include "graphics.h"
#include "character.h"
#include "struct.h"
#include "textureManager.h"
#include "rlgl.h"
#include "raymath.h"
#include <vector>
#include <algorithm>
#include <utility>

struct RenderObject
{
	float distance_sqr = 0.0f;
	object* object_ptr = nullptr;
	enemy* enemy_ptr = nullptr;
};

std::pair<float, int> GetOutlineParams(BodyPart part, float frameHeight)
{
	switch (part)
	{
	case BodyPart::TORSO:     return { 0.0f * frameHeight, 0 };
	case BodyPart::HEAD:      return { 1.0f * frameHeight, 0 };
	case BodyPart::RIGHT_ARM: return { 2.0f * frameHeight, 1 };
	case BodyPart::LEFT_ARM:  return { 2.0f * frameHeight, 2 };
	case BodyPart::RIGHT_LEG: return { 3.0f * frameHeight, 1 };
	case BodyPart::LEFT_LEG:  return { 3.0f * frameHeight, 2 };
	default:                  return { 0.0f, 0 };
	}
}

void DrawOutlineBillboard(Camera3D camera, Texture2D texture, Shader shader, Rectangle rec, Vector3 pos, Vector2 size, int sideLimit)
{
	rlDisableDepthMask();

	float texSize[2] = { (float)texture.width, (float)texture.height };
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	SetShaderValue(shader, GetShaderLocation(shader, "textureSize"), texSize, SHADER_UNIFORM_VEC2);
	SetShaderValue(shader, GetShaderLocation(shader, "outlineColor"), color, SHADER_UNIFORM_VEC4);
	SetShaderValue(shader, GetShaderLocation(shader, "sideLimit"), &sideLimit, SHADER_UNIFORM_INT);

	SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
	BeginShaderMode(shader);
	DrawBillboardRec(camera, texture, rec, pos, size, WHITE);
	EndShaderMode();
	SetTextureFilter(texture, TEXTURE_FILTER_POINT);

	rlEnableDepthMask();
}

void DrawExploration(exploration* exp)
{
	dungeon_floor& current_floor = exp->floors[exp->current_floor_id];

	int promienWidzenia = 20;

	int playerGridX = (int)(exp->camera.position.x / 2.0f);
	int playerGridY = (int)(exp->camera.position.z / 2.0f);

	int startX = std::max(0, playerGridX - promienWidzenia);
	int endX = std::min(exp->szerokosc, playerGridX + promienWidzenia);
	int startY = std::max(0, playerGridY - promienWidzenia);
	int endY = std::min(exp->dlugosc, playerGridY + promienWidzenia);

	for (int cx = startX; cx < endX; cx++)
	{
		for (int cy = startY; cy < endY; cy++)
		{
			int tile = current_floor.dungeon[cx][cy];

			if (tile == 2)
			{
				Vector3 pozycja_dolna = Vector3{ (float)cx * 2.0f, 0.0f, (float)cy * 2.0f };
				Vector3 pozycja_gorna = Vector3{ (float)cx * 2.0f, 2.0f, (float)cy * 2.0f };

				// SCIANA POLUDNIOWA
				if (cy > 0 && current_floor.dungeon[cx][cy - 1] == 1)
				{
					DrawModelEx(objects.wall_tile, pozycja_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 0.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					DrawModelEx(objects.wall_tile, pozycja_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 0.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
				}
				// SCIANA POLNOCNA
				if (cy < exp->dlugosc - 1 && current_floor.dungeon[cx][cy + 1] == 1)
				{
					Vector3 p_dolna = Vector3{ pozycja_dolna.x - 2.0f, 0.0f, pozycja_dolna.z + 2.0f };
					Vector3 p_gorna = Vector3{ pozycja_gorna.x - 2.0f, 2.0f, pozycja_gorna.z + 2.0f };
					DrawModelEx(objects.wall_tile, p_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 180.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					DrawModelEx(objects.wall_tile, p_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 180.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
				}
				// SCIANA WSCHODNIA
				if (cx > 0 && current_floor.dungeon[cx - 1][cy] == 1)
				{
					Vector3 p_dolna = Vector3{ pozycja_dolna.x - 2.0f, 0.0f, pozycja_dolna.z };
					Vector3 p_gorna = Vector3{ pozycja_gorna.x - 2.0f, 2.0f, pozycja_gorna.z };
					DrawModelEx(objects.wall_tile, p_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 90.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					DrawModelEx(objects.wall_tile, p_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 90.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
				}
				// SCIANA ZACHODNIA
				if (cx < exp->szerokosc - 1 && current_floor.dungeon[cx + 1][cy] == 1)
				{
					Vector3 p_dolna = Vector3{ pozycja_dolna.x, 0.0f, pozycja_dolna.z + 2.0f };
					Vector3 p_gorna = Vector3{ pozycja_gorna.x, 2.0f, pozycja_gorna.z + 2.0f };
					DrawModelEx(objects.wall_tile, p_dolna, Vector3{ 0.0f, 1.0f, 0.0f }, 270.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
					DrawModelEx(objects.wall_tile, p_gorna, Vector3{ 0.0f, 1.0f, 0.0f }, 270.0f, Vector3{ 1.0f, 1.0f, 1.0f }, WHITE);
				}
			}
			else if (tile == 1)
			{
				DrawModel(objects.floor_tile, Vector3{ (float)cx * 2.0f, 0.0f, (float)cy * 2.0f }, 1.0f, WHITE);
				DrawModel(objects.ceiling_tile, Vector3{ (float)cx * 2.0f, 4.0f, (float)cy * 2.0f }, 1.0f, WHITE);
			}
		}
	}

	std::vector<RenderObject> object_to_draw;

	Vector3 enemyForward = { 0.0f, 0.0f, 1.0f };

	for (const auto& object : current_floor.world_objects)
	{
		if (!object) continue;
		int GridX = (int)(object->position.x / 2.0f);
		int GridY = (int)(object->position.z / 2.0f);

		if (GridX >= startX && GridX < endX && GridY >= startY && GridY < endY)
		{
			float dx = object->position.x - exp->camera.position.x;
			float dy = object->position.y - exp->camera.position.y;
			float dz = object->position.z - exp->camera.position.z;

			RenderObject renderObject;
			renderObject.object_ptr = object.get();
			renderObject.distance_sqr = (dx * dx) + (dy * dy) + (dz * dz);
			object_to_draw.push_back(renderObject);
		}
	}

	// Draw active enemies on the current floor
	for (enemy* e : current_floor.active_enemies)
	{
		if (e != nullptr && !e->is_dead())
		{
			Vector3 pos = e->get_position();
			int enemyGridX = (int)(pos.x / 2.0f);
			int enemyGridY = (int)(pos.z / 2.0f);

			if (enemyGridX >= startX && enemyGridX < endX &&
				enemyGridY >= startY && enemyGridY < endY)
			{
				float dx = pos.x - exp->camera.position.x;
				float dy = pos.y - exp->camera.position.y;
				float dz = pos.z - exp->camera.position.z;

				RenderObject renderObject;
				renderObject.enemy_ptr = e;
				renderObject.distance_sqr = (dx * dx) + (dy * dy) + (dz * dz);
				object_to_draw.push_back(renderObject);
			}
		}
	}
	std::sort(object_to_draw.begin(), object_to_draw.end(), [](const RenderObject& a, const RenderObject& b)
		{
			return a.distance_sqr > b.distance_sqr;
		});

	for (const auto& item : object_to_draw)
	{
		if (item.object_ptr != nullptr)
		{
			item.object_ptr->draw(exp->camera);
		}
		else if (item.enemy_ptr != nullptr)
		{
			auto* enemy = item.enemy_ptr;
			int current_id = enemy->get_id();
			Vector3 pos = enemy->get_position();

			switch (current_id)
			{
			case 1:
			{
				auto params = enemy->get_render_params(textures.ghoul);

				int lastFrame = enemy->get_last_frame();
				int frameIndex = GetSpriteFrameIndex(pos, enemy->get_forward(), exp->camera.position, lastFrame);
				enemy->set_last_frame(frameIndex);

				BodyPart hoveredPart = enemy->get_hovered_body_part();
				const auto& limbs = enemy->limbs;

				SetShaderValue(textures.fogShader, GetShaderLocation(textures.fogShader, "viewPos"), &exp->camera.position, SHADER_UNIFORM_VEC3);

				int sideLimitLoc = GetShaderLocation(textures.fogShader, "sideLimit");
				int noLimit = 0;

				BeginShaderMode(textures.fogShader);

				if (limbs.left_arm.is_intact || limbs.right_arm.is_intact)
				{
					SetShaderValue(textures.fogShader, sideLimitLoc, &noLimit, SHADER_UNIFORM_INT);
					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 2 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.ghoul, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}

				if (limbs.torso.is_intact)
				{
					SetShaderValue(textures.fogShader, sideLimitLoc, &noLimit, SHADER_UNIFORM_INT);
					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 0 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.ghoul, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}

				if (limbs.head.is_intact)
				{
					SetShaderValue(textures.fogShader, sideLimitLoc, &noLimit, SHADER_UNIFORM_INT);
					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 1 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.ghoul, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}

				if (limbs.left_leg.is_intact || limbs.right_leg.is_intact)
				{
					int legSideLimit = 0;
					if (!limbs.left_leg.is_intact)       legSideLimit = 1;
					else if (!limbs.right_leg.is_intact) legSideLimit = 2;

					int adjustedLimit = enemy->GetAdjustedSideLimit(legSideLimit, frameIndex);
					SetShaderValue(textures.fogShader, sideLimitLoc, &adjustedLimit, SHADER_UNIFORM_INT);

					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 3 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.ghoul, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();

					SetShaderValue(textures.fogShader, sideLimitLoc, &noLimit, SHADER_UNIFORM_INT);
				}

				EndShaderMode();

				if (hoveredPart != BodyPart::NONE)
				{
					auto [rowY, sideLimit] = GetOutlineParams(hoveredPart, params.frameHeight);
					Rectangle outlineRec = { frameIndex * params.frameWidth, rowY, params.frameWidth, params.frameHeight };

					DrawOutlineBillboard(exp->camera, textures.ghoul, textures.outlineShader, outlineRec, params.drawPos, { params.targetWidth, params.targetHeight }, sideLimit);
				}
				break;
			}
			case 2:
			{
				auto params = enemy->get_render_params(textures.pimpek);

				int lastFrame = enemy->get_last_frame();
				int frameIndex = GetSpriteFrameIndex(pos, enemy->get_forward(), exp->camera.position, lastFrame);
				enemy->set_last_frame(frameIndex);

				BodyPart hoveredPart = enemy->get_hovered_body_part();
				const auto& limbs = enemy->limbs;

				SetShaderValue(textures.fogShader, GetShaderLocation(textures.fogShader, "viewPos"), &exp->camera.position, SHADER_UNIFORM_VEC3);

				int sideLimitLoc = GetShaderLocation(textures.fogShader, "sideLimit");
				int noLimit = 0;

				BeginShaderMode(textures.fogShader);

				if (limbs.torso.is_intact)
				{
					SetShaderValue(textures.fogShader, sideLimitLoc, &noLimit, SHADER_UNIFORM_INT);
					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 0 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.pimpek, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}
				if (limbs.left_arm.is_intact || limbs.right_arm.is_intact)
				{
					SetShaderValue(textures.fogShader, sideLimitLoc, &noLimit, SHADER_UNIFORM_INT);
					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 2 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.pimpek, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}
				if (limbs.head.is_intact)
				{
					SetShaderValue(textures.fogShader, sideLimitLoc, &noLimit, SHADER_UNIFORM_INT);
					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 1 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.pimpek, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();
				}

				if (limbs.left_leg.is_intact || limbs.right_leg.is_intact)
				{
					int legSideLimit = 0;
					if (!limbs.left_leg.is_intact)       legSideLimit = 1;
					else if (!limbs.right_leg.is_intact) legSideLimit = 2;

					int adjustedLimit = enemy->GetAdjustedSideLimit(legSideLimit, frameIndex);
					SetShaderValue(textures.fogShader, sideLimitLoc, &adjustedLimit, SHADER_UNIFORM_INT);

					Rectangle rowSourceRec = { frameIndex * params.frameWidth, 3 * params.frameHeight, params.frameWidth, params.frameHeight };
					DrawBillboardRec(exp->camera, textures.pimpek, rowSourceRec, params.drawPos, { params.targetWidth, params.targetHeight }, WHITE);
					rlDrawRenderBatchActive();

					SetShaderValue(textures.fogShader, sideLimitLoc, &noLimit, SHADER_UNIFORM_INT);
				}

				EndShaderMode();

				if (hoveredPart != BodyPart::NONE)
				{
					auto [rowY, sideLimit] = GetOutlineParams(hoveredPart, params.frameHeight);
					if (hoveredPart == BodyPart::LEFT_ARM || hoveredPart == BodyPart::RIGHT_ARM)
					{
						sideLimit = 0;
					}
					Rectangle outlineRec = { frameIndex * params.frameWidth, rowY, params.frameWidth, params.frameHeight };

					DrawOutlineBillboard(exp->camera, textures.pimpek, textures.outlineShader, outlineRec, params.drawPos, { params.targetWidth, params.targetHeight }, sideLimit);
				}
				break;
			}
			}
		}
	}
}

void draw_game_scene(exploration* exp)
{
	ClearBackground(BLACK);

	int viewPosLoc = GetShaderLocation(textures.fogShader, "viewPos");
	SetShaderValue(textures.fogShader, viewPosLoc, &exp->camera.position, SHADER_UNIFORM_VEC3);

	BeginMode3D(exp->camera);

	DrawExploration(exp);

	EndMode3D();

	exp->update_and_draw_floating_texts(exp->camera);

	DrawHUD(exp);
}

void DrawGlobalAnimation()
{
	if (!global_fx.is_playing)
	{
		return;
	}

	float scale = 4.0f;
	float frameWidth = (float)global_fx.texture.width / global_fx.frame_count;
	float frameHeight = (float)global_fx.texture.height;

	global_fx.position = { 1920.0f * 0.5f, 1080.0f * 0.5f };

	Rectangle sourceRec = {
		global_fx.current_frame * frameWidth,
		0.0f,
		frameWidth,
		frameHeight
	};

	Rectangle destRec = {
		global_fx.position.x,
		global_fx.position.y,
		frameWidth * scale,
		frameHeight * scale
	};

	Vector2 origin = {
		(frameWidth * scale) * 0.5f,
		(frameHeight * scale) * 0.5f
	};

	DrawTexturePro(global_fx.texture, sourceRec, destRec, origin, 0.0f, WHITE);
}

void draw_map_tile(exploration* exp, int x, int y, int start_x, int start_y, int tile)
{
	int offset_x = start_x + (tile * x);
	int offset_y = start_y + (tile * y);

	DrawRectangle(offset_x, offset_y, tile, tile, DARKGREEN);
}

void draw_dungeon_map(exploration* exp, float player_x, float player_y)
{
	if (exp == nullptr || exp->floors.empty() ||
		exp->current_floor_id < 0 || exp->current_floor_id >= static_cast<int>(exp->floors.size()))
	{
		return;
	}

	const auto& current_dungeon = exp->floors[exp->current_floor_id].dungeon;

	DrawRectangle(100, 100, 1080, 520, Fade(BLACK, 0.8f));
	DrawRectangleLines(100, 100, 1080, 520, RAYWHITE);

	int tile = 10;
	int vision_range = 20;

	float exact_player_x_pos = player_x / 2.0f;
	float exact_player_y_pos = player_y / 2.0f;

	int player_x_pos_int = static_cast<int>(std::floor(exact_player_x_pos));
	int player_y_pos_int = static_cast<int>(std::floor(exact_player_y_pos));

	int center_box_x = 640;
	int center_box_y = 360;

	int start_x = center_box_x - static_cast<int>(exact_player_x_pos * tile);
	int start_y = center_box_y - static_cast<int>(exact_player_y_pos * tile);

	int min_x = std::max(0, player_x_pos_int - vision_range);
	int max_x = std::min(exp->szerokosc - 1, player_x_pos_int + vision_range);

	int min_y = std::max(0, player_y_pos_int - vision_range);
	int max_y = std::min(exp->dlugosc - 1, player_y_pos_int + vision_range);

	for (int x = min_x; x <= max_x; x++)
	{
		for (int y = min_y; y <= max_y; y++)
		{
			if (current_dungeon[x][y] == 1)
			{
				draw_map_tile(exp, x, y, start_x, start_y, tile);
			}
		}
	}

	DrawCircle(center_box_x, center_box_y, 4.0f, YELLOW);
}
