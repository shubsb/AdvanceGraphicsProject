#version 330 core

uniform vec3 u_color; //rgb
uniform vec4 u_lightPosDir[8]; //W comp is "bool"
uniform vec3 u_cameraPos; //camera position
uniform int u_numLights; //lights in scene
uniform float u_shininess; //specular shininess term
uniform float u_lightIntensity[8]; //intensity of light

in vec3 surfaceNormal;
in vec3 worldPosition;

void main() {
	vec3 N = normalize(surfaceNormal);
	vec3 V = normalize(u_cameraPos - worldPosition);

	//phong - ASD (Ambient, Specular, Diffuse)
	float ambient = 0.2f;
	float diffuse = 0.0f;
	float specular = 0.0f;

	//set light direction
	for(int i = 0; i < u_numLights; i++){
		vec3 L = vec3(0.0f);
			//L = normalize(u_lightPosDir[i].xyz - worldPosition);
		L = normalize(-u_lightPosDir[i].xyz);
		//phong
		vec3 R = reflect(-L,N);
			//just phong
			float specularAngle = max(0.0f, dot(V, R));
			specular += pow(specularAngle, u_shininess / 4.0f)*u_lightIntensity[i];


		diffuse += max(0.0f, dot(N, L))*u_lightIntensity[i];
	}

    gl_FragColor = vec4(u_color*(ambient+diffuse+specular), 1.0);
	//gl_FragColor = vec4(N, 1.0);
	//gl_FragColor = vec4(u_color,1.0);
}
