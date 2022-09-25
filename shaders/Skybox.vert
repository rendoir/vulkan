#version 450

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_uv0;
layout (location = 3) in vec2 i_uv1;

// Camera set
layout (set = 0, binding = 0) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 position;
} u_camera;

layout (location = 0) out vec3 o_uvw;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main() 
{
	o_uvw = i_position;
	mat4 viewNoTranslation = mat4(mat3(u_camera.view));
	gl_Position = u_camera.projection * viewNoTranslation * vec4(i_position.xyz, 1.0);
}
