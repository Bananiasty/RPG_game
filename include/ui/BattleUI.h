#pragma once
#include "raylib.h"

struct limb;
class player;
class battle;

class BodyStatusUI
{
public:
    Texture2D tex_silhouette;
    Texture2D tex_mask;
    Shader shader;

    int loc_mask;
    int loc_head;
    int loc_torso;
    int loc_left_arm;
    int loc_right_arm;
    int loc_left_leg;
    int loc_right_leg;

    int get_status_code(const limb& l);

    void init();
    void draw(const player& p, Vector2 position, float scale);
    void cleanup();
};

extern BodyStatusUI body_UI;

void draw_battle_ui(battle* fight);
