#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 textureSize;   
uniform vec4 outlineColor;
uniform float outlineSize;

void main()
{
    vec4 texel = texture(texture0, fragTexCoord);

    if (texel.a > 0.4) 
    {
    	finalColor = texel * fragColor;
    }
    else
    {
        vec2 size = vec2(textureSize.x * 8.0, textureSize.y); 
        vec2 offset = (vec2(1.0) / size) * outlineSize;

        vec2 uvUp    = fragTexCoord + vec2(0.0, -offset.y);
        vec2 uvDown  = fragTexCoord + vec2(0.0,  offset.y);
        vec2 uvLeft  = fragTexCoord + vec2(-offset.x, 0.0);
        vec2 uvRight = fragTexCoord + vec2( offset.x, 0.0);

        float a1 = (uvUp.y >= 0.0)    ? texture(texture0, uvUp).a    : 0.0;
        float a2 = (uvDown.y <= 1.0)  ? texture(texture0, uvDown).a  : 0.0;
        float a3 = texture(texture0, uvLeft).a;
        float a4 = texture(texture0, uvRight).a;

        if (a1 > 0.1 || a2 > 0.1 || a3 > 0.1 || a4 > 0.1)
        {
            finalColor = outlineColor;
        }
        else
        {
            discard;
        }
    }
}