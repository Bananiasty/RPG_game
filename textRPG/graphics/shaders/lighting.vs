#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;

uniform mat4 mvp;
uniform mat4 matModel;

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    
    // Pozycja wierzchołka w świecie 3D
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}