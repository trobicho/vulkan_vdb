#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_DIST	10
#define MIN_DIST	0.001
#define	MAX_STEP	1000

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

float	sphere_dist(vec3 p)
{
	return (length(p) - 0.2);
}

float	marching(vec3 ro, vec3 rd)
{
	float	d = 0.;
	float	ds = 0.;
	vec3	p;

	for(int i = 0; i < MAX_STEP; i++)
	{
		p = ro + (rd * d);
		ds = sphere_dist(p);
		d += ds;
		if (d > MAX_DIST)
			return (MAX_STEP);
		if (ds <= MIN_DIST)
			return (i);
	}
	return (MAX_STEP);
}

void main()
{
	float	d;
	vec3	ro = vec3(0.0, 0.0, -10.0);
	vec2	coord = gl_FragCoord.xy / vec2(800, 800) - vec2(0.5, 0.5);
	vec3	rd = normalize(vec3(coord, 0.0) - ro);
	float	iter;
	float	col;

	outColor = vec4(rd, 1.0);
}
