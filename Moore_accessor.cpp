/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Moore_accessor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/23 04:53:19 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/13 16:45:38 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Moore_accessor.h"
#include "Block.h"

Moore_accessor::Moore_accessor(Vdb_test &vdb): m_vdb(vdb)
{
	m_neigh.reset();
}

void	Moore_accessor::find_neigh(s_vec3i v, Node_v *node)
{
	s_vec3i vox;
	m_neigh.reset();
	s_vec3i	log = node->get_log();
	int nb_are = 0;
	int nb_not = 0;
	uint32_t	block = m_vdb.get_vox(v);
	uint32_t	found;
	bool		is_opaque = block_is_opaque(block);

	std::bitset<6>	face;
	face.reset();
	if (node->is_leaf())
	{
		face[0] = !((v.y & ((1 << log.y) - 1)) == 0);
		face[1] = !((v.y & ((1 << log.y) - 1)) == ((1 << log.y) - 1));
		face[2] = !((v.z & ((1 << log.z) - 1)) == 0);
		face[3] = !((v.z & ((1 << log.z) - 1)) == ((1 << log.z) - 1));
		face[4] = !((v.x & ((1 << log.x) - 1)) == 0);
		face[5] = !((v.x & ((1 << log.x) - 1)) == ((1 << log.x) - 1));
	}

	for (int y = 0; y < 3; y++)
	{
		for (int z = 0; z < 3; z++)
		{
			for (int x = 0; x < 3; x++)
			{
				vox.x = v.x + (x - 1);
				vox.y = v.y + (y - 1);
				vox.z = v.z + (z - 1);
				if (node->is_leaf())
				{
					if ((y == 1 || (y == 0 && face[0]) || (y == 2 && face[1]))
						&& (x == 1 || (x == 0 && face[4]) || (x == 2 && face[5]))
						&& (z == 1 || (z == 0 && face[2]) || (z == 2 && face[3])))
					{
						if ((found = node->get_vox(vox.x, vox.y, vox.z)))
						{
							m_neigh[y * 9 + z * 3 + x] = (is_opaque == block_is_opaque(found));
						}
						continue;
					}
					else if ((found = m_vdb.get_vox(vox)))
						m_neigh[y * 9 + z * 3 + x] = (is_opaque == block_is_opaque(found));
				}
				else if ((found = m_vdb.get_vox(vox)))
					m_neigh[y * 9 + z * 3 + x] = (is_opaque == block_is_opaque(found));
			}
		}
	}
	//std::cout << nb_are << ", " << nb_not << std::endl;
}
