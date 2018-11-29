#version 330 core
uniform vec3 u_LightPos;
//uniform vec4 u_lightPosDir[8];
uniform vec4 u_DiffuseColour;
uniform vec3 u_cameraPos; //camera position
uniform float u_shininess; //specular shininess term

//uniform int u_numLights; //lights in scene

//uniform float u_lightIntensity[8]; //intensity of light

//uniform vec3 u_color; //rgb


//in vec3 surfaceNormal;
//in vec3 worldPosition;

in vec3 v_Position;
in vec3 v_Normal;

void main() {
	vec4 ambientColour = vec4(0.1, 0.1, 0.1, 1.0);

	// Will be used for attenuation.
	float distance = length(u_LightPos - v_Position);

	// Get a lighting direction vector from the light to the vertex.
	vec3 L = normalize(u_LightPos - v_Position);

	// Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
	// pointing in the same direction then it will get max illumination.
	vec3 N = normalize(v_Normal);
	float diffuse = clamp(dot(N, L), 0, 1);

	// Add attenuation.
	diffuse = diffuse * (1.0 / (1.0 + (0.00025 * distance * distance)));
	vec3 incidenceVector = -L;
  vec3 reflectionVector = reflect(incidenceVector, v_Normal);
	vec3 eyeVector = normalize(u_cameraPos - v_Position);
	float cosAngle = max(0.0, dot(eyeVector, reflectionVector));
	float specularCoefficient = pow(cosAngle, u_shininess);

	// should be (diffuse * specular) * attenuationFactor;

	// Multiply the color by the diffuse illumination level to get final output color.
	gl_FragColor = specularCoefficient * u_DiffuseColour + u_DiffuseColour * diffuse + ambientColour;
}
