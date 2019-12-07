/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Space_colonisation.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 01:42:49 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/07 05:09:13 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Space_colonisation.h"
#include "Block.h"
#include <iostream>

Space_colonisation::Space_colonisation(s_space_col_info &info, Vdb_test &vdb):
	m_branch(info.branch), m_attractor(info.attractor)
	, m_di(info.di), m_dk(info.dk)
	, m_vdb(vdb)
{
}

void	Space_colonisation::grow_full(int nb_step)
{
	for (int i = 0; i < nb_step; i++)
	{
		step();
	}
	for (auto &b : m_branch)
	{
		if (!b.has_grow())
			add_leaf(b);
	}
}

void	Space_colonisation::add_leaf(Branch &branch)
{
	s_vec3i		vox;
	s_vec3i		len = s_vec3i(3, 3, 3);

	vox = s_vec3i((int)branch.pos.x - len.x / 2
			, (int)branch.pos.y - len.y / 2, (int)branch.pos.z - len.z / 2);
	for (int x = 0; x < len.x; x++)
	{
		for (int z = 0; z < len.z; z++)
		{
			for (int y = 0; y < len.y; y++)
			{
				s_vec3i v = vox;
				v.x += x;
				v.y += y;
				v.z += z;
				if (m_vdb.get_vox(v) == 0)
					m_vdb.set_vox(bl_leaf, v);
			}
		}
	}
}

void	Space_colonisation::step()
{
	size_t	size_branch = m_branch.size();
	size_t	nb_stop = 0;

	for (auto &b : m_branch)
	{
		b.reset();
		if (b.has_stop())
			nb_stop++;
	}
	std::cout << "stop: " << nb_stop << "/" << m_branch.size() << std::endl;
	for (auto const &a : m_attractor)
	{
		float	min = m_di + 0.1;
		size_t	min_id = size_branch + 1;
		float	len;
		for (size_t b = 0; b < size_branch; ++b)
		{
			if (m_branch[b].has_stop() == false
				&& (len = glm::length(a - m_branch[b].pos)) < m_di)
			{
				m_branch[b].is_influance();
				if (len < min)
				{
					min = len;
					min_id = b;
				}
			}

		}
		if (min_id < size_branch)
			m_branch[min_id].add_to_vec(a - m_branch[min_id].pos);
	}
	for (size_t b = 0; b < size_branch; ++b)
	{
		if (m_branch[b].can_grow())
		{
			Branch	new_branch = m_branch[b].grow();
			new_branch.link = b;
			int l = b;
			while (m_branch[l].link != l)
			{
				m_branch[l].width++;
				l = m_branch[l].link;
			}
			s_vec3i	vox = s_vec3i((int)new_branch.pos.x
					, (int)new_branch.pos.y, (int)new_branch.pos.z);
			m_vdb.set_vox(bl_wood, vox);
			m_branch.push_back(new_branch);
		}
		else if (m_branch[b].has_infuance() == false)
			m_branch[b].stop();
	}

	for (int n = (m_first ? 0 : size_branch); n < m_branch.size(); ++n)
	{
		int a = 0;
		while (a < m_attractor.size())
		{
			if (glm::length(m_attractor[a] - m_branch[n].pos) < m_dk)
			{
				m_attractor.erase(m_attractor.begin() + a);
				continue;
			}
			a++;
		}
		m_first = false;
	}
}

Branch		Branch::grow()
{
	float		d = 1.0;
	Branch		new_branch(this->pos);


	if (m_grow_count > 0)
	{
		m_has_grow = true;
		m_grow_vec /= m_grow_count;
		m_grow_vec = glm::normalize(m_grow_vec);
		new_branch.pos += m_grow_vec * d;
	}
	return (new_branch);
}
