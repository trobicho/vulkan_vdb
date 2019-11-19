#version 450
#extension GL_ARB_separate_shader_objects : enable

#define M_PI                3.14159265

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

layout(binding = 0) uniform UniformBufferObject {
	mat4	model;
	mat4	view;
	mat4	proj;
} ubo;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 1.0);
	float len = length(gl_Position) / 10.0;
	out_color = vec4(vec3(1.0f, 1.0f, 1.0f) / ((len * len * (M_PI)) / 20), 1.0) / 2.0;
	out_color = max(out_color, vec4(0.1));
	out_color = min(out_color, vec4(1.0));
	out_color *= vec4(in_color, 1.0f);
}
