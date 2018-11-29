#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 textureCoords;

//uniform mat4 u_MVP;
//attribute vec4 position;
//attribute vec2 textureCoords;
//attribute vec3 normal;
//uniform mat4 u_MVPMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 surfaceNormal;
out vec3 worldPosition;

void main() {
  gl_Position = projection * view * model * vec4(aPos,1.0);
	worldPosition = gl_Position.xyz;
	surfaceNormal = normal;
}
