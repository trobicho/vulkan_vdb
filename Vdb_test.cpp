/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vdb_test.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 21:06:27 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/24 19:57:05 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Vdb_test.h"

Vdb_test::Vdb_test(): m_min(0, 0, 0), m_max(8192, 8192, 8192)
{
}

void		Vdb_test::set_vox(uint32_t value, s_vec3i v)
{
	if (v.x >= m_min.x && v.y >= m_min.y && v.z >= m_min.z
		&& v.x < m_max.x && v.y < m_max.y && v.z < m_max.z)
	{
		m_root_static.set_vox(value, v.x, v.y, v.z);
	}
}

uint32_t	Vdb_test::get_vox(s_vec3i v)
{
	if (v.x >= m_min.x && v.y >= m_min.y && v.z >= m_min.z
		&& v.x < m_max.x && v.y < m_max.y && v.z < m_max.z)
	{
		return (m_root_static.get_vox(v.x, v.y, v.z));
	}
	return (0);
}

const Node_v
			*Vdb_test::get_interresting_node(s_vec3i v, uint32_t &value)
{
	value = 0;
	if (v.x >= m_min.x && v.y >= m_min.y && v.z >= m_min.z
		&& v.x < m_max.x && v.y < m_max.y && v.z < m_max.z)
	{
		return (m_root_static.get_interresting_node(v, value));
	}
	return (nullptr);
}

void		Vdb_test::pruning()
{
	m_root_static.pruning();
}

void		Vdb_test::mesh(Mesh &mesh) const
{
	m_root_static.mesh(mesh);
}

void		Vdb_test::mesh(Mesh &mesh, const s_vbox &box) const
{
	
	if (box.origin.x >= m_min.x
		&& box.origin.y >= m_min.y 
		&& box.origin.z >= m_min.z
		&& box.origin.x + box.len.x < m_max.x
		&& box.origin.y + box.len.y < m_max.y 
		&& box.origin.z + box.len.z < m_max.z)
	{
		m_root_static.mesh(mesh, box);
	}
}
