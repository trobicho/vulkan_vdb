/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 18:38:50 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/20 02:25:00 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include "my_lib.h"
#include "My_vulkan.h"
#include "perlin.h"

enum e_biome_type
{
	bt_tundra,
	bt_borreal_forest,
	bt_grassland,
	bt_shrubland,
	bt_temperate_forest,
	bt_temperate_rainforest,
	bt_subtropical_desert,
	bt_savanna,
	bt_tropical_rainforest,
};

struct	s_vbox
{
	s_vec3i	origin;
	s_vec3i	len;
};

struct	s_biome_info
{
	double			temp;
	double			prec;
	e_biome_type	type;
};

class	Map
{
	public:
		Map(uint32_t seed): m_seed(seed){};

		int			generate(Vdb_test &vdb, s_vbox box);
		uint64_t	get_nb_vox(){return (m_nb_vox);}

	private:
		double			get_height(double x, double y);
		double			get_density_cave(double x, double y, double z);
		void			get_biome_info(s_biome_info &biome_info
							, double x, double z);
		uint32_t		get_block_type(s_biome_info &biome_info, double y
							, double height);
		e_biome_type	get_biome_type(double temp, double prec);

		uint32_t		m_seed;
		uint64_t		m_nb_vox = 0;
		Perlin_noiser	m_noise;

};
