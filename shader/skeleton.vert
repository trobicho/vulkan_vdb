#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4		out_color;
layout(location = 1) out float		out_dist_obs;

layout(location = 0) in vec3		in_pos;
layout(location = 1) in vec4		in_color;
layout(location = 2) in	uint 		in_bones_index;
layout(location = 3) in float		in_bones_weight;
layout(location = 4) in lowp int	in_ao;

layout(binding = 0) uniform UniformBufferObject {
	mat4	model;
	mat4	view;
	mat4	proj;
	vec4	pos_enemy;
	mat4	bone[17];
} ubo;

void main() {
	vec4 new_pos = ubo.bone[in_bones_index] * vec4(in_pos, 1.0)
		* in_bones_weight * 0.1;
	new_pos += ubo.pos_enemy;
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(new_pos.xyz, 1.0);

	vec3	color_scalar = vec3(1.0) / (4 - in_ao);
	vec3	color;

	color.x = in_color.x * ((in_bones_index + 1) % 2);
	color.y = in_color.y * ((in_bones_index) % 3);
	color.z = in_color.z * ((in_bones_index) % 2);

	out_color = vec4((color * color_scalar), in_color.a);
	out_dist_obs = length(gl_Position);
}
