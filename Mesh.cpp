/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 07:00:55 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/26 00:05:08 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.h"
#include "Moore_accessor.h"

Mesh::Mesh(Moore_accessor &moore_access): m_moore_access(moore_access)
{
}

void	Mesh::update()
{
	m_old_size = vertex_buffer.size();
}

void	Mesh::reset()
{
	vertex_buffer.clear();
	index_buffer.clear();
}

int		Mesh::add_vertex(s_vertex v, uint32_t idx)
{
	vertex_buffer.push_back(v);
	index_buffer.push_back(idx);
	return (vertex_buffer.size() - 1);
}

int		Mesh::add_vertex_with_basic_index(s_vertex v)
{
	vertex_buffer.push_back(v);
	index_buffer.push_back(vertex_buffer.size() - 1);
	return (vertex_buffer.size() - 1);
}

int		Mesh::add_vertex_with_no_index(s_vertex v, uint8_t ao)
{
	v.ao = ao;
	vertex_buffer.push_back(v);
	return (vertex_buffer.size() - 1);
}

void	Mesh::add_index(uint32_t idx)
{
	index_buffer.push_back(idx);
}

void	Mesh::remove_vertex(uint32_t offset, uint32_t size)
{
	bool	del;

	if (offset + size >= vertex_buffer.size())
		return ;
	vertex_buffer.erase(vertex_buffer.begin() + offset
			, vertex_buffer.begin() + offset + size);
	for (int t = 0; t < index_buffer.size();)
	{
		del = false;
		for (int i = 0; i < 3; i++)
		{
			if (index_buffer[t + i] >= offset
				&& index_buffer[t + i] < offset + size)
			{
				index_buffer.erase(index_buffer.begin() + t
					, index_buffer.begin() + t + 3);
				del = true;
				break;
			}
			else if (index_buffer[t + i] >= offset + size)
				index_buffer[t + i] -= size;
		}
		if (!del)
			t += 3;
	}
}


void	Mesh::get_needed_vertex(std::bitset<8> &v_b)
{
	v_b.reset();
	if (!m_moore_access[MOORE_UP])
		v_b |= ((1 << 4) - 1);
	if (!m_moore_access[MOORE_DOWN])
		v_b |= ((1 << 4) - 1) << 4;
	if (!m_moore_access[MOORE_FRONT])
	{
		v_b |= ((((1 << 2) - 1) << 4) | ((1 << 2) - 1));
	}
	if (!m_moore_access[MOORE_BACK])
	{
		v_b |= ((((1 << 2) - 1) << 6) | (((1 << 2) - 1) << 2));
	}
	if (!m_moore_access[MOORE_RIGHT])
	{
		v_b |= ((((1 << 2) - 1) << 5) | (((1 << 2) - 1) << 1));
	}
	if (!m_moore_access[MOORE_LEFT])
	{
		v_b |= ~((((1 << 2) - 1) << 5) | (((1 << 2) - 1) << 1));
	}
}

void	Mesh::add_needed_vertex(s_vertex v, uint32_t l, std::bitset<8> &v_b
			, uint32_t v_idx[8])
{
	s_vertex	vc = v;

	if (v_b[0])
		v_idx[0] = add_vertex_with_no_index(vc, get_ao(0));
	vc.pos.x += l;
	if (v_b[1])
		v_idx[1] = add_vertex_with_no_index(vc, get_ao(1));
	vc.pos.z += l;
	if (v_b[2])
		v_idx[2] = add_vertex_with_no_index(vc, get_ao(2));
	vc.pos.x -= l;
	if (v_b[3])
		v_idx[3] = add_vertex_with_no_index(vc, get_ao(3));

	vc = v;
	vc.pos.y += l;
	if (v_b[4])
		v_idx[4] = add_vertex_with_no_index(vc, get_ao(4));
	vc.pos.x += l;
	if (v_b[5])
		v_idx[5] = add_vertex_with_no_index(vc, get_ao(5));
	vc.pos.z += l;
	if (v_b[6])
		v_idx[6] = add_vertex_with_no_index(vc, get_ao(6));
	vc.pos.x -= l;
	if (v_b[7])
		v_idx[7] = add_vertex_with_no_index(vc, get_ao(7));
}

void	Mesh::add_cube_from_node(s_vec3i v, e_block_type type, void *node_ptr)
{
	std::bitset<8>	v_b;
	uint32_t		v_idx[8];
	s_vertex		vertex(glm::vec3((float)v.x, (float)v.y, (float)v.z)
						, get_color_from_block_type(type));
	Node_v			*node = (Node_v*)node_ptr;

	m_moore_access.find_neigh(v, node);
	get_needed_vertex(v_b);
	add_needed_vertex(vertex, 1 << node->get_child_slog().x, v_b, v_idx); // weird
	if (!m_moore_access[MOORE_UP])
	{
		add_index(v_idx[2]);
		add_index(v_idx[1]);
		add_index(v_idx[0]);

		add_index(v_idx[0]);
		add_index(v_idx[3]);
		add_index(v_idx[2]);
	}
	if (!m_moore_access[MOORE_DOWN])
	{
		add_index(v_idx[4]);
		add_index(v_idx[5]);
		add_index(v_idx[6]);

		add_index(v_idx[6]);
		add_index(v_idx[7]);
		add_index(v_idx[4]);
	}
	if (!m_moore_access[MOORE_FRONT])
	{
		add_index(v_idx[0]);
		add_index(v_idx[1]);
		add_index(v_idx[5]);

		add_index(v_idx[5]);
		add_index(v_idx[4]);
		add_index(v_idx[0]);
	}
	if (!m_moore_access[MOORE_BACK])
	{
		add_index(v_idx[6]);
		add_index(v_idx[2]);
		add_index(v_idx[3]);

		add_index(v_idx[3]);
		add_index(v_idx[7]);
		add_index(v_idx[6]);
	}
	if (!m_moore_access[MOORE_RIGHT])
	{
		add_index(v_idx[5]);
		add_index(v_idx[1]);
		add_index(v_idx[2]);

		add_index(v_idx[2]);
		add_index(v_idx[6]);
		add_index(v_idx[5]);
	}
	if (!m_moore_access[MOORE_LEFT])
	{
		add_index(v_idx[7]);
		add_index(v_idx[3]);
		add_index(v_idx[0]);

		add_index(v_idx[0]);
		add_index(v_idx[4]);
		add_index(v_idx[7]);
	}
}

uint8_t		Mesh::get_ao(int idx)
{
	switch (idx)
	{
		case 0:	
			return (vertex_ao(m_moore_access[3], m_moore_access[1]
					, m_moore_access[0]));
		case 1:	
			return (vertex_ao(m_moore_access[1], m_moore_access[5]
					, m_moore_access[2]));
		case 2:	
			return (vertex_ao(m_moore_access[7], m_moore_access[5]
					, m_moore_access[8]));
		case 3:	
			return (vertex_ao(m_moore_access[7], m_moore_access[3]
					, m_moore_access[6]));
		case 4:	
			return (vertex_ao(m_moore_access[21], m_moore_access[19]
					, m_moore_access[18]));
		case 5:	
			return (vertex_ao(m_moore_access[19], m_moore_access[23]
					, m_moore_access[20]));
		case 6:	
			return (vertex_ao(m_moore_access[25], m_moore_access[23]
					, m_moore_access[26]));
		case 7:	
			return (vertex_ao(m_moore_access[25], m_moore_access[21]
					, m_moore_access[24]));
		default:
			return (3);
	}
}
