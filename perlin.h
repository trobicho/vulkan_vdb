/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/16 17:58:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/16 20:14:37 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.h"

class	Perlin_noiser
{
	public:
		Perlin_noiser(){};

		double	lerp(double a, double b, double t);
		double	cerp(double a, double b, double t);
		double	cubic_interpolate(double p0, double p1
					, double b_p0, double a_p1, double t);
		double	noise_2d(int x, int y);
		double	smooth_noise(double x);
		double	smooth_noise2D(double x, double y);
		double	perlin(int nb_octave, double freq
			, double persistance, double x);

	private:
}
