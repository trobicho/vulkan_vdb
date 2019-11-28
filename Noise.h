/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Noise.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 13:35:25 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/28 13:44:12 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class	Noise
{
	public:
		Noise(){};
		~Noise(){};

		static double	lerp(double a, double b, double t);
		double			smooth_noise3d(double x_pos, double y_pos, double z_pos);
};
