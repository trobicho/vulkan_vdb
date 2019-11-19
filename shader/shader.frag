#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 in_color;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D tex_sampler;

void main() {
	out_color = in_color;
	//vec4(pow(out_color.xyz, vec3(1 / 2.2))
}
