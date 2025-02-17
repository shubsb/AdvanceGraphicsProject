#version 330 core
uniform vec4 u_DiffuseColour;
uniform vec3 u_LightPosDir[8];
uniform vec3 u_EyePosition;
uniform float u_Shininess;
uniform int numLights;

in vec3 v_Position;
in vec3 v_Normal;

void main() {
     vec4 ambientColour = vec4(0.1, 0.1, 0.1, 1.0);

     // distance between the fragment and the light source (for attenuation)
     float distance = length(u_LightPos - v_Position);

     // vector from fragment to light source
     vec3 light_vector = normalize(-u_LightPos);

     // diffuse component

     // calculate the diffuse contribution
     vec3 normal = normalize(v_Normal);
     float diffuse = clamp(dot(normal, light_vector), 0, 1);

     // attenuate
     diffuse *= (1.0 / (1.0 + (0.000000000000000025 * distance * distance)));

     // specular component
     vec3 incidence_vector = -light_vector;
     vec3 reflection_vector = reflect(incidence_vector, normal);
     vec3 eye_vector = normalize(u_EyePosition - v_Position);
     float cosAngle = clamp(dot(eye_vector, reflection_vector), 0, 1);
     float specular = pow(cosAngle, u_Shininess);

   gl_FragColor = vec4(1.0f, 0.8f, 0.2f, 1.0f) *(specular * u_DiffuseColour + diffuse * u_DiffuseColour + ambientColour);
   //gl_FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
