/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 07:00:55 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/22 23:40:00 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.h"

Mesh::Mesh()
{
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

int		Mesh::add_vertex_with_no_index(s_vertex v)
{
	vertex_buffer.push_back(v);
	return (vertex_buffer.size() - 1);
}

void	Mesh::add_index(uint32_t idx)
{
	index_buffer.push_back(idx);
}

void	Mesh::get_needed_vertex(std::bitset<8> &v_b
			, std::bitset<6> &moore_neigh)
{
	v_b.reset();
	if (!moore_neigh[0])
		v_b |= ((1 << 4) - 1);
	if (!moore_neigh[1])
		v_b |= ((1 << 4) - 1) << 4;
	if (!moore_neigh[2])
	{
		v_b |= ((((1 << 2) - 1) << 4) | ((1 << 2) - 1));
	}
	if (!moore_neigh[3])
	{
		v_b |= ((((1 << 2) - 1) << 6) | (((1 << 2) - 1) << 2));
	}
	if (!moore_neigh[4])
	{
		v_b |= ((((1 << 2) - 1) << 5) | (((1 << 2) - 1) << 1));
	}
	if (!moore_neigh[5])
	{
		v_b |= ~((((1 << 2) - 1) << 5) | (((1 << 2) - 1) << 1));
	}
}

void	Mesh::add_needed_vertex(s_vertex v, uint32_t w
			, std::bitset<8> &v_b, uint32_t v_idx[8])
{
	s_vertex	vc = v;

	if (v_b[0])
		v_idx[0] = add_vertex_with_no_index(vc);
	vc.pos.x += w;
	if (v_b[1])
		v_idx[1] = add_vertex_with_no_index(vc);
	vc.pos.z += w;
	if (v_b[2])
		v_idx[2] = add_vertex_with_no_index(vc);
	vc.pos.x -= w;
	if (v_b[3])
		v_idx[3] = add_vertex_with_no_index(vc);

	vc = v;
	vc.pos.y += w;
	if (v_b[4])
		v_idx[4] = add_vertex_with_no_index(vc);
	vc.pos.x += w;
	if (v_b[5])
		v_idx[5] = add_vertex_with_no_index(vc);
	vc.pos.z += w;
	if (v_b[6])
		v_idx[6] = add_vertex_with_no_index(vc);
	vc.pos.x -= w;
	if (v_b[7])
		v_idx[7] = add_vertex_with_no_index(vc);
}

void	Mesh::add_cube_moore(s_vertex v, uint32_t w
			, std::bitset<6> &moore_neigh)
{
	std::bitset<8>	v_b;
	uint32_t		v_idx[8];

	get_needed_vertex(v_b, moore_neigh);
	add_needed_vertex(v, w, v_b, v_idx);
	if (!moore_neigh[0])
	{
		add_index(v_idx[2]);
		add_index(v_idx[1]);
		add_index(v_idx[0]);

		add_index(v_idx[0]);
		add_index(v_idx[3]);
		add_index(v_idx[2]);
	}
	if (!moore_neigh[1])
	{
		add_index(v_idx[4]);
		add_index(v_idx[5]);
		add_index(v_idx[6]);

		add_index(v_idx[6]);
		add_index(v_idx[7]);
		add_index(v_idx[4]);
	}
	if (!moore_neigh[2])
	{
		add_index(v_idx[0]);
		add_index(v_idx[1]);
		add_index(v_idx[5]);

		add_index(v_idx[5]);
		add_index(v_idx[4]);
		add_index(v_idx[0]);
	}
	if (!moore_neigh[3])
	{
		add_index(v_idx[6]);
		add_index(v_idx[2]);
		add_index(v_idx[3]);

		add_index(v_idx[3]);
		add_index(v_idx[7]);
		add_index(v_idx[6]);
	}
	if (!moore_neigh[4])
	{
		add_index(v_idx[5]);
		add_index(v_idx[1]);
		add_index(v_idx[2]);

		add_index(v_idx[2]);
		add_index(v_idx[6]);
		add_index(v_idx[5]);
	}
	if (!moore_neigh[5])
	{
		add_index(v_idx[7]);
		add_index(v_idx[3]);
		add_index(v_idx[0]);

		add_index(v_idx[0]);
		add_index(v_idx[4]);
		add_index(v_idx[7]);
	}
}
