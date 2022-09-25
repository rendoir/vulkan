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

// Transform set
layout (set = 1, binding = 0) uniform Transform
{
	mat4 model;
} u_transform;

layout (location = 0) out vec3 o_worldPosition;
layout (location = 1) out vec3 o_normal;
layout (location = 2) out vec2 o_uv0;
layout (location = 3) out vec2 o_uv1;

void main() 
{
	o_worldPosition = vec3(u_transform.model * vec4(i_position, 1.0));
    o_normal = normalize(transpose(inverse(mat3(u_transform.model))) * i_normal);
	o_uv0 = i_uv0;
	o_uv1 = i_uv1;
	gl_Position =  u_camera.projection * u_camera.view * vec4(o_worldPosition, 1.0);
}
