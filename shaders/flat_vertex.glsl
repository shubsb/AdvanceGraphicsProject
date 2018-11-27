#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 normal;

uniform mat4 u_MVMatrix;
uniform mat4 u_MVPMatrix;

out vec3 v_Position;
out vec3 v_Normal;

void main() {
   // interpolate the position and normal
   v_Position = vec3(u_MVMatrix * vec4(aPos,1.0));
   v_Normal = vec3(u_MVMatrix * vec4(normal, 0.0));
   


   // set the vertex's position
   gl_Position = u_MVPMatrix * vec4(aPos,1.0);
}
