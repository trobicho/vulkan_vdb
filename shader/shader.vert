#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 out_tex_coord;
layout(location = 1) out vec4 out_color;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex_coord;

layout(binding = 0) uniform UniformBufferObject {
	mat4	model;
	mat4	view;
	mat4	proj;
} ubo;

#define M_PI                3.14159265

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 1.0);
	out_tex_coord = in_tex_coord;
	float len = length(gl_Position) / 20.0;
	out_color = vec4(vec3(1.0f, 1.0f, 1.0f) / ((len * len * (M_PI)) / 15.0), 1.0);
	out_color = max(out_color, vec4(0.2));
	out_color = min(out_color, vec4(1.4));
}
