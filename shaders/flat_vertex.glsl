#version 330 core
layout (location = 0)

uniform mat4 u_MVP;
in vec3 aPos; // the position variable has attribute position 0

attribute vec4 position;
attribute vec2 textureCoords;
attribute vec3 normal;

out vec4 vertexColor; // specify a color output to the fragment shader
out vec3 surfaceNormal;
out vec3 worldPosition;

void main()
{
    gl_Position = u_MVP * position;
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
    worldPosition = gl_Position.xyz;
    surfaceNormal = normal;
}
