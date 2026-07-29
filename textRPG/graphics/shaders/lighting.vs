#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;

uniform mat4 mvp;

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;


    vec4 eyePos = mvp * vec4(vertexPosition, 1.0);
    fragPosition = eyePos.xyz;

    gl_Position = eyePos;
}