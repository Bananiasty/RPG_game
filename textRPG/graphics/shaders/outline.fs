#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 textureSize;
uniform vec4 outlineColor;
uniform int sideLimit;

void main()
{
    // Ograniczenie dla lewej / prawej strony
    float frameX = fract(fragTexCoord.x * 8.0);
    if (sideLimit == 1 && frameX > 0.5) discard;
    if (sideLimit == 2 && frameX < 0.5) discard;

    vec4 texel = texture(texture0, fragTexCoord);

    // Odrzucamy samą postać, aby pod spodem było widać ostrego ghoula z wywołania (B)
    if (texel.a > 0.4) 
    {
        discard;
    }

    vec2 offset = vec2(1.2 / textureSize.x, 1.2 / textureSize.y);
    float alphaSum = 0.0;

    // Próbkowanie otoczenia z wagami Gaussowskimi
    alphaSum += texture(texture0, fragTexCoord + vec2(-offset.x, -offset.y)).a * 0.5;
    alphaSum += texture(texture0, fragTexCoord + vec2( 0.0,      -offset.y)).a * 1.0;
    alphaSum += texture(texture0, fragTexCoord + vec2( offset.x, -offset.y)).a * 0.5;

    alphaSum += texture(texture0, fragTexCoord + vec2(-offset.x,  0.0)).a * 1.0;
    alphaSum += texture(texture0, fragTexCoord + vec2( offset.x,  0.0)).a * 1.0;

    alphaSum += texture(texture0, fragTexCoord + vec2(-offset.x,  offset.y)).a * 0.5;
    alphaSum += texture(texture0, fragTexCoord + vec2( 0.0,       offset.y)).a * 1.0;
    alphaSum += texture(texture0, fragTexCoord + vec2( offset.x,  offset.y)).a * 0.5;

    float avgAlpha = alphaSum / 6.0;

    // MAGIA ANTI-ALIASINGU:
    // fwidth wyznacza natężenie zmian na ekranie i wygładza dokładnie brzegi pikseli
    float delta = fwidth(avgAlpha);
    float glowAlpha = smoothstep(0.05 - delta, 0.45 + delta, avgAlpha);

    if (glowAlpha > 0.01)
    {
        // Przypisanie alfy z miękkim wygaszaniem brzegów
        finalColor = vec4(outlineColor.rgb, outlineColor.a * glowAlpha);
    }
    else
    {
        discard;
    }
}