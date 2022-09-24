#version 450

layout (location = 0) in vec3 i_uvw;

layout (set = 1, binding = 2) uniform samplerCube u_skyboxCube;

layout (location = 0) out vec4 o_color;

// Constants
layout (constant_id = 0) const float c_gamma = 2.2;
layout (constant_id = 1) const float c_exposure = 4.5;

#include "Common/ColorSpace.glsl"

void main() 
{
	vec3 color = textureLod(u_skyboxCube, i_uvw, 1.5).rgb;
	ApplyHDRTonemapping(color);
	ApplyGammaCorrection(color);

	o_color = vec4(color, 1.0);
}
