#version 450

layout (location = 0) in vec3 i_position;

layout(push_constant) uniform Transformation {
	layout (offset = 0) mat4 mvp;
} pc_transformation;

layout (location = 0) out vec3 o_uvw;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{
	o_uvw = i_position;
	gl_Position = pc_transformation.mvp * vec4(i_position.xyz, 1.0);
}
