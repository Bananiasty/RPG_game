#include "struct.h"
#include "battle_UI.h"
#include "raylib.h"
#include "rlgl.h"


int BodyStatusUI::get_status_code(const limb& l)
{
    if (!l.is_intact) return 3;               // Odcięta
    if (l.hp <= 0) return 2;                  // Zniszczona
    if (l.hp < (l.max_hp * 0.8f)) return 1;   // Ranna
    return 0;                                 // Zdrowa
}
void BodyStatusUI::init()
    {
        tex_silhouette = LoadTexture("graphics/UI/man_silhouette.png");
        tex_mask = LoadTexture("graphics/UI/man_body_mask.png");

        shader = LoadShader(nullptr, "graphics/shaders/bodystatus.fs");

        // Pobranie lokacji zmiennych z pliku .fs
        loc_mask = GetShaderLocation(shader, "maskTexture");

        loc_head = GetShaderLocation(shader, "head_status");
        loc_torso = GetShaderLocation(shader, "torso_status");
        loc_left_arm = GetShaderLocation(shader, "left_arm_status");
        loc_right_arm = GetShaderLocation(shader, "right_arm_status");
        loc_left_leg = GetShaderLocation(shader, "left_leg_status");
        loc_right_leg = GetShaderLocation(shader, "right_leg_status");

        // Przypisanie maski do slotu tekstury nr 1
        int mask_texture_slot = 1;
        SetShaderValue(shader, loc_mask, &mask_texture_slot, SHADER_UNIFORM_INT);
        
    }

    void BodyStatusUI::draw(const player& p, Vector2 position, float scale)
    {

        // 1. Wyliczenie statusów na podstawie obiektu gracza
        int head_st = get_status_code(p.limbs.head);
        int torso_st = get_status_code(p.limbs.torso);
        int larm_st = get_status_code(p.limbs.left_arm);
        int rarm_st = get_status_code(p.limbs.right_arm);
        int lleg_st = get_status_code(p.limbs.left_leg);
        int rleg_st = get_status_code(p.limbs.right_leg);

        // 2. Przesłanie wartości do shadera
        SetShaderValue(shader, loc_head, &head_st, SHADER_UNIFORM_INT);
        SetShaderValue(shader, loc_torso, &torso_st, SHADER_UNIFORM_INT);
        SetShaderValue(shader, loc_left_arm, &larm_st, SHADER_UNIFORM_INT);
        SetShaderValue(shader, loc_right_arm, &rarm_st, SHADER_UNIFORM_INT);
        SetShaderValue(shader, loc_left_leg, &lleg_st, SHADER_UNIFORM_INT);
        SetShaderValue(shader, loc_right_leg, &rleg_st, SHADER_UNIFORM_INT);

        // 3. Rysowanie sylwetki z aktywnym shaderem i drugą teksturą
        BeginShaderMode(shader);

        rlActiveTextureSlot(1);
        rlEnableTexture(tex_mask.id);

        rlActiveTextureSlot(0);
        DrawTextureEx(tex_silhouette, position, 0.0f, scale, WHITE);

        rlDrawRenderBatchActive();

        rlActiveTextureSlot(1);
        rlDisableTexture();

        rlActiveTextureSlot(0);

        EndShaderMode();
    }

    void BodyStatusUI::cleanup()
    {
        UnloadTexture(tex_silhouette);
        UnloadTexture(tex_mask);
        UnloadShader(shader);
    }