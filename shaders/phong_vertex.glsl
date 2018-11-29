#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 normal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 v_Position;
out vec3 v_Normal;

void main() {
   // interpolate the position and normal
   v_Position = vec3(model*view * vec4(aPos,1.0));
   v_Normal = vec3(model*view * vec4(normal, 0.0));



   // set the vertex's position
   gl_Position = projection * view * model * vec4(aPos,1.0);
}
