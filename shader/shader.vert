#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 out_tex_coord;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex_coord;

layout(binding = 0) uniform UniformBufferObject {
	mat4	model;
	mat4	view;
	mat4	proj;
} ubo;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 1.0);
	out_tex_coord = in_tex_coord;
}
