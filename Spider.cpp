/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 22:08:47 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/25 01:31:00 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Spider.h"

Spider::Spider(): m_mesh(m_moore_access)
{
}

void	Spider::add_box(s_vbox box, uint32_t value)
{
	for (int x = 0; x < box.len.x; x++)
	{
		for (int y = 0; y < box.len.y; y++)
		{
			for (int z = 0; z < box.len.z; z++)
			{
				m_vdb.set_vox(value, s_vec3i(box.origin.x + x
					, box.origin.y + y, box.origin.z + z));
			}
		}
	}
}

void	Spider::generate()
{
	s_vbox	body;
	s_vbox	leg;

	m_bones.resize(17);
	body.origin = s_vec3i(50, 47, 1);
	body.len = s_vec3i(28, 16, 55);
	add_box(body, 1);
	for (int i = 0; i < 4; i++)
	{
		leg.origin = s_vec3i(0, 50, 3 + i * ((body.len.z - 8) / 4.0));
		leg.len = s_vec3i(50, 3, 3);
		add_box(leg, 2);
		leg.origin.x = 78;
		add_box(leg, 2);
	}
	m_vdb.pruning();
	m_vdb.mesh(m_mesh);
}
