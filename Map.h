/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 18:38:50 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/19 19:05:28 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include "my_lib.h"
#include "My_vulkan.h"
#include "perlin.h"

struct	s_vbox
{
	s_vec3i	origin;
	s_vec3i	len;
};


class	Map
{
	public:
		Map(uint32_t seed): m_seed(seed){};

		int			generate(Vdb_test &vdb, s_vbox box);
		uint64_t	get_nb_vox(){return (m_nb_vox);}

	private:
		double		get_height(double x, double y);
		double		get_density_cave(double x, double y, double z);
		int			get_biome_type(double x, double z);

		uint32_t		m_seed;
		uint64_t		m_nb_vox = 0;
		Perlin_noiser	m_noise;

};
