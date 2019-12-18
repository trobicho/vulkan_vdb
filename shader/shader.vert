#version 450
#extension GL_ARB_separate_shader_objects : enable

#define M_PI                3.14159265

layout(location = 0) out vec4		out_color;
layout(location = 1) out float		out_dist_obs;

layout(location = 0) in vec3		in_pos;
layout(location = 1) in vec4		in_color;
layout(location = 2) in lowp int	in_ao;

layout(binding = 0) uniform UniformBufferObject {
	mat4	model;
	mat4	view;
	mat4	proj;
	vec3	sun_pos;
} ubo;

void main()
{

	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 1.0);
	//float intensity = 1000;
	//float len = length(ubo.sun_pos - in_pos) / 10.0f;
	//len *= len;
	//out_color = vec4(vec3(1.0f, 1.0f, 1.0f) * (intensity / (M_PI * len)), 1.0f);
	vec3	color_scalar = vec3(1.0) / (4 - in_ao);
	out_color = vec4((in_color.xyz * color_scalar), in_color.a);
	out_dist_obs = length(gl_Position);
}
