#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 in_color;
layout(location = 1) in float in_dist_obs;

layout(location = 0) out vec4 out_color;

vec3 fog(vec3 color, vec3 fcolor, float depth, float density){
    const float e = 2.71828182845904523536028747135266249;
    float f = pow(e, - pow(depth * density, 2));
    return mix(fcolor, color, f);
}

void main() {
	out_color = vec4(fog(in_color.xyz, vec3(0.8), in_dist_obs, 0.005), in_color.a);
	//vec4(pow(out_color.xyz, vec3(1 / 2.2))
}
