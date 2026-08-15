#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D maskTexture;

uniform int head_status;
uniform int torso_status;
uniform int left_arm_status;
uniform int right_arm_status;
uniform int left_leg_status;
uniform int right_leg_status;

out vec4 finalColor;

const vec3 COLOR_HEAD      = vec3(1.0, 0.0, 0.0); // Czerwony
const vec3 COLOR_TORSO     = vec3(0.0, 0.0, 1.0); // Niebieski
const vec3 COLOR_LEFT_ARM  = vec3(0.0, 1.0, 1.0); // Cyjan
const vec3 COLOR_RIGHT_ARM = vec3(1.0, 0.0, 1.0); // Magenta
const vec3 COLOR_LEFT_LEG  = vec3(0.0, 1.0, 0.0); // Zielony
const vec3 COLOR_RIGHT_LEG = vec3(1.0, 1.0, 0.0); // Żółty

void main()
{
    vec4 baseTex = texture(texture0, fragTexCoord);
    vec4 maskTex = texture(maskTexture, fragTexCoord);

    if (baseTex.a < 0.1 || maskTex.a < 0.1)
    {
        finalColor = vec4(0.0);
        return;
    }

    int current_status = 0;

    if (distance(maskTex.rgb, COLOR_HEAD) < 0.2)           current_status = head_status;
    else if (distance(maskTex.rgb, COLOR_TORSO) < 0.2)     current_status = torso_status;
    else if (distance(maskTex.rgb, COLOR_LEFT_ARM) < 0.2)  current_status = left_arm_status;
    else if (distance(maskTex.rgb, COLOR_RIGHT_ARM) < 0.2) current_status = right_arm_status;
    else if (distance(maskTex.rgb, COLOR_LEFT_LEG) < 0.2)  current_status = left_leg_status;
    else if (distance(maskTex.rgb, COLOR_RIGHT_LEG) < 0.2) current_status = right_leg_status;

    vec3 pixelColor = baseTex.rgb;
    float pixelAlpha = baseTex.a;

    if (current_status == 1)      // Ranna
    {
        pixelColor = vec3(0.60, 0.48, 0.08);
    }
    else if (current_status == 2) // Zniszczona
    {
       pixelColor = vec3(0.50, 0.06, 0.06);
    }
    else if (current_status == 3) // Odcięta
    {
        pixelAlpha = 0.0;
    }

    finalColor = vec4(pixelColor, pixelAlpha) * fragColor;
}