#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 viewPos; 
uniform float minDistance;
uniform float maxDistance;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord) * colDiffuse * fragColor;
	
    if (texelColor.a < 0.1) 
    {
	discard;
    }
    float distance = length(fragPosition);

    float factor = (maxDistance - distance) / (maxDistance - minDistance);
    factor = clamp(factor, 0.0, 1.0);
    
    finalColor = vec4(texelColor.rgb * factor, texelColor.a);
}