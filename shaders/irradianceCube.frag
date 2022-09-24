#version 450

layout (location = 0) in vec3 i_position;
layout (location = 0) out vec4 o_color;

layout (set = 0, binding = 0) uniform samplerCube u_samplerEnv;

layout(push_constant) uniform IrradianceSettings {
	layout (offset = 64) float deltaPhi;
	layout (offset = 68) float deltaTheta;
} pc_settings;

#include "Common/Constants.glsl"

void main()
{
	vec3 N = normalize(i_position);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = cross(N, right);

	vec3 color = vec3(0.0);
	uint sampleCount = 0u;
	for (float phi = 0.0; phi < TWO_PI; phi += pc_settings.deltaPhi) {
		for (float theta = 0.0; theta < HALF_PI; theta += pc_settings.deltaTheta) {
			// Spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // Tangent space to world space
            vec3 sampleVector = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
			
			color += texture(u_samplerEnv, sampleVector).rgb * cos(theta) * sin(theta);
			sampleCount++;
		}
	}

	o_color = vec4(PI * color / float(sampleCount), 1.0);
}
