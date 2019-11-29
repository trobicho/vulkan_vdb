#version 450
#extension GL_ARB_separate_shader_objects : enable

#define M_PI                3.14159265

layout(location = 0) out vec4	out_color;

layout(location = 0) in vec3			in_pos;
layout(location = 1) in vec4			in_color;
layout(location = 2) in lowp int		in_ao;

layout(binding = 0) uniform UniformBufferObject {
	mat4	model;
	mat4	view;
	mat4	proj;
	vec3	sun_pos;
} ubo;

void main()
{

	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 1.0);
	out_color = vec4(in_color);
}
