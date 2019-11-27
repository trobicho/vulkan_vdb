/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Moore_accessor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/23 04:53:19 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/27 14:25:56 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Moore_accessor.h"

Moore_accessor::Moore_accessor(Vdb_test &vdb): m_vdb(vdb)
{
	m_neigh.reset();
}

void	Moore_accessor::find_neigh(s_vec3i v, Node_v *node)
{
	s_vec3i vox;
	m_neigh.reset();
	for (int y = 0; y < 3; y++)
	{
		for (int z = 0; z < 3; z++)
		{
			for (int x = 0; x < 3; x++)
			{
				vox.x = v.x + (x - 1);
				vox.y = v.y + (y - 1);
				vox.z = v.z + (z - 1);
				if (m_vdb.get_vox(vox))
					m_neigh[y * 9 + z * 3 + x] = true;
			}
		}
	}
}
