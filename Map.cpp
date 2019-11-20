/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 18:43:08 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/20 03:11:52 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Map.h"
#include "Block.h"

double		Map::get_height(double x, double z)
{
	double	scalar = 500.0;
	double	d = m_noise.perlin2d(5, 2., 0.8
			, (double)x / scalar
			, (double)z / scalar);
	return (d);
}

double		Map::get_density_cave(double x, double y, double z)
{
	double	scalar_cave = 30.0;
	double	d = m_noise.perlin3d(3, 1.5, 0.5
			, (double)x / scalar_cave
			, (double)z / scalar_cave
			, (double)y / scalar_cave);
	return (d);

}

void		Map::get_biome_info(s_biome_info &biome_info, double x, double z)
{
	double	scalar_temp = 100.0;
	double	scalar_prec = 100.0;

	biome_info.temp = m_noise.perlin2d(4, 1.5, 0.8
			, (double)x / scalar_temp
			, (double)z / scalar_temp);
	biome_info.prec = m_noise.perlin2d(5, 2., 0.8
			, (double)(10000 - x) / scalar_prec
			, (double)z / scalar_prec);
	biome_info.type = get_biome_type(biome_info.temp, biome_info.prec);
}

uint32_t	Map::get_block_type(s_biome_info &biome_info, double y
				, double height)
{
	if (biome_info.type == bt_tundra)
		return (bl_ice);
	if (y < 0.45)
		return (bl_stone);
	if (biome_info.type == bt_grassland
		|| biome_info.type == bt_shrubland || biome_info.type == bt_savanna)
		return (bl_sand);
	return (bl_dirt);
}

int			Map::generate(Vdb_test &vdb, s_vbox box)
{
	double			cave_thres = 0.3;
	double			cave_thres_d;
	int				lerp_mod = 8;
	double			d_cave_prec, d_cave_next, d_cave;
	int				block_type = 1;
	s_biome_info	biome_info;
	double			d;
	int				y;

	for (int z = 0; z < box.len.z; ++z)
	{
		for (int x = 0; x < box.len.x; ++x)
		{
			get_biome_info(biome_info, x + box.origin.x, z + box.origin.z);
			for (y = 0; y < box.len.y * d + 1; ++y)
			{
				s_vec3i	vox(x, y, z);
				d = get_height(x + box.origin.x, z + box.origin.z);
				if ((y - 1) % lerp_mod == 0)
				{
					d_cave = get_density_cave(x + box.origin.x
							, y + box.origin.y, z + box.origin.z);
					d_cave_prec = d_cave;
					d_cave_next = get_density_cave(x + box.origin.x
							, y + box.origin.y + lerp_mod, z + box.origin.z);
				}
				else if (y > 0)
				{
					d_cave = Perlin_noiser::lerp(d_cave_prec, d_cave_next
								, (y % lerp_mod) / (double)lerp_mod);
				}
				cave_thres_d = (1.0 - (double)y / box.len.y) - cave_thres;
				if (y > 0 && d_cave < cave_thres)
					continue ;
				block_type = get_block_type(biome_info, (double)y / box.len.y, d);
				if (block_type == bl_dirt && y >= box.len.y * d - 1)
					block_type = bl_grass;
				vox.x += box.origin.x;
				vox.y += box.origin.y;
				vox.z += box.origin.z;
				vdb.set_vox(block_type, vox);
				m_nb_vox++;
			}
			for (; y < box.len.y * 0.45 + 1; ++y)
			{
				s_vec3i	vox(x, y, z);
				vox.x += box.origin.x;
				vox.y += box.origin.y;
				vox.z += box.origin.z;
				vdb.set_vox((uint32_t)((e_block_type)bl_water), vox);
			}
		}
	}
	return (0);
}

e_biome_type	Map::get_biome_type(double temp, double prec)
{
	e_biome_type	type;

	if (temp < 0.250)
		type = bt_tundra;
	else if (temp <= 0.45)
	{
		if (prec > 0.3)
			type = bt_borreal_forest;
		else if (prec < 0.2)
			type = bt_grassland;
		else
			type = bt_shrubland;
	}
	else if (temp < 0.75)
	{
		if (prec > 0.8)
			type = bt_temperate_rainforest;
		else if (prec > 0.45)
			type = bt_temperate_forest;
		else if (prec < 0.1 && temp > 0.60)
			type = bt_subtropical_desert;
		else if (prec < 0.22)
			type = bt_grassland;
		else
			type = bt_shrubland;
	}
	else
	{
		if (prec < 0.2)
			type = bt_subtropical_desert;
		else if (prec < 0.65)
			type = bt_savanna;
		else
			type = bt_tropical_rainforest;
	}
	return (type);
}
