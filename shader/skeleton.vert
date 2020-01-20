#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4		out_color;
layout(location = 1) out float		out_dist_obs;

layout(location = 0) in vec3		in_pos;
layout(location = 1) in vec4		in_color;
layout(location = 2) in	int 		in_bones_index;
layout(location = 3) in int			in_bones_weight;
layout(location = 4) in lowp int	in_ao;

layout(binding = 0) uniform UniformBufferObject {
	mat4	model;
	mat4	view;
	mat4	proj;
	mat4	Bone[17];
} ubo;

void main() {
	vec4 new_pos = Bone[in_bones_index] * in_pos * in_bones_weight.x;
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(new_pos.xyz, 1.0);
	vec3	color_scalar = vec3(1.0) / (4 - in_ao);
	out_color = vec4((in_color.xyz * color_scalar), in_color.a);
	out_dist_obs = length(gl_Position);
}
