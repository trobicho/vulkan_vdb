/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 18:43:08 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/19 21:35:55 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Map.h"

double	Map::get_height(double x, double z)
{
	double	scalar = 500.0;
	double	d = m_noise.perlin2d(5, 2., 0.8
			, (double)x / scalar
			, (double)z / scalar);
	return (d);
}

double	Map::get_density_cave(double x, double y, double z)
{
	double	scalar_cave = 30.0;
	double	d = m_noise.perlin3d(3, 1.5, 0.5
			, (double)x / scalar_cave
			, (double)z / scalar_cave
			, (double)y / scalar_cave);
	return (d);

}

int		Map::get_biome_type(double x, double y)
{
	return (1);
}

int		Map::generate(Vdb_test &vdb, s_vbox box)
{
	double			cave_thres = 0.3;
	double			cave_thres_d;
	int				lerp_mod = 8;
	double			d_cave_prec, d_cave_next, d_cave;
	int				block_type = 1;
	double			d;

	for (int z = 0; z < box.len.z; ++z)
	{
		for (int x = 0; x < box.len.x; ++x)
		{
			block_type = get_biome_type(x + box.origin.x, z + box.origin.z);
			block_type = 2;
			for (int y = 0; y < box.len.y * d + 1; ++y)
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
				if (y >= box.len.y * d - 2)
					block_type = 1;
				vox.x += box.origin.x;
				vox.y += box.origin.y;
				vox.z += box.origin.z;
				vdb.set_vox(block_type, vox);
				m_nb_vox++;
				if (m_nb_vox > 0 && m_nb_vox % 200000000 == 0)
				{
					vdb.pruning();
					std::cout << "prunning" << std::endl;
					std::cout << (z / (double)box.len.x) * 100.0 << "%"  << std::endl;
					std::cout << "total of " << m_nb_vox << " voxels." << std::endl;
				}
			}
		}
	}
	return (0);
}

