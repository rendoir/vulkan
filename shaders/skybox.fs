#version 450

layout (binding = 1) uniform samplerCube samplerEnv;

layout (set = 0, location = 0) in vec3 inUVW;

layout (set = 0, location = 0) out vec4 outColor;

// Constants
const float exposure = 4.5;
const float gamma = 2.2;

// Uncharted2 Tonemapping
vec3 Uncharted2Tonemap(vec3 x) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;

	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 applyHDRTonemapping(in vec3 color) {
	float W = 11.2;

	vec3 current = Uncharted2Tonemap(color * exposure);
	vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));

	return current * whiteScale;
}

vec3 linearTosRGB(vec3 linearIn) {
	return pow(linearIn, vec3(1.0 / gamma));
}

void main() 
{
	outColor = vec4(linearTosRGB(applyHDRTonemapping(textureLod(samplerEnv, inUVW, 1.5).rgb)), 1.0);
}