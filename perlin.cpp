/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/16 18:03:36 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/16 20:20:04 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmath>

double	Perlin_noiser::rand_noise(int t)
{
	t = (t << 13) ^ t;
	t = (t * (t * t * 15731 + 789221) + 1376312589);
	return (1.0 - (t & 0x7fffffff) / 1073741824.0;
}

double	Perlin_noiser::lerp(double a, double b, double t)
{
	return ((1. - t) * a + t * b);
}

double	Perlin_noiser::cerp(double a, double b, double t)
{
	double	c = (1. - cos(t * M_PI)) * 0.5;
	return ((1. - c) * a + c * b);
}

double	Perlin_noiser::cubic_interpolate(double p0, double p1
			, double b_p0, double a_p1, double t)
{
	double	c3 = -0.5 * b_p0 + 1.5 * p0 - 1.5 * p1 + 0.5 * a_p1;
	double	c2 = b_p0 - 2.5 * p0 + 2. * p1 - 0.5 * a_p1; 
	double	c1 = -0.5 * b_p0 + 0.5 * p1;
	double	c0 = p0;

	return ((c3 * t * t * t) + (c2 * t * t) + (c1 * t) + c0);
}

double	Perlin_noiser::noise_2d(int x, int y)
{
	int	tmp = rand_noise(x) * 850000;

	return (tmp + y);
}

double	Perlin_noiser::smooth_noise(double x)
{
	int		int_x = ((x >= 0) ? (int)x : (int)x - 1);
	double	frac_x = x - int_x;
	double	a = noise(int_x);
	double	b = noise(int_x + 1);

	return (lerp(a, b, frac_x));
}

double	Perlin_noiser::smooth_noise2D(double x, double y)
{
	int		int_x = ((x >= 0) ? (int)x : (int)x - 1);
	int		int_y = ((y >= 0) ? (int)y : (int)y - 1);
	double	frac_x = x - int_x;
	double	frac_y = y - int_y;
	double	a, b, c, d;
	double	f, g;

	a = noise2D(int_x, int_y);
	b = noise2D(int_x + 1, int_y);
	c = noise2D(int_x, int_y + 1);
	d = noise2D(int_x + 1, int_y + 1);

	f = lerp(a, b, fract_x);
	f = lerp(c, d, fract_x);

	return (lerp(f, g, frac_y));
}

double	Perlin_noiser::perlin(int nb_octave, double freq
			, double persistence, double x)
{
	double	r = 0.;
	double	f = freq;
	double	amplitude = 1.;

	for (int i = 0; i < nb_octave; i++)
	{
		int t = i * 4096;
		r += smooth_noise(x * f + t) * amplitude;
		amplitude *= persistence;
		f *= 2.;
	}
	double	geo_lim = (1 - persistence) / (1 - amplitude);
	return (r * geo_lim);
}
