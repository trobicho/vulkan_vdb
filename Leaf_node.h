/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Leaf_node.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 20:38:57 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/12 14:22:29 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <bitset>
#include "Node.h"

template <class Value, int Log2X, int Log2Y = Log2X, int Log2Z = Log2Y>
class Leaf_node: public Node<Value>
{
	public:
		Leaf_node(int32_t x, int32_t y, int32_t z);
		~Leaf_node(){};


		void		do_set_vox(Value v, int32_t x, int32_t y, int32_t z);
		Value		do_get_vox(int32_t x, int32_t y, int32_t z) const;
		Value		pruning();
		const Node<Value>
					*do_get_interresting_node(s_vec3i v, Value &value) const;
		void		do_mesh(Mesh &mesh) const;

		static const int sSize = 1 << (Log2X + Log2Y + Log2Z);
		static const int sLog2X = Log2X, sLog2Y = Log2Y, sLog2Z = Log2Z;

	private:
		inline bool			do_is_leaf() const {
			return (true);
		}
		inline s_vec3i		do_get_log() const {
			return s_vec3i(Log2X, Log2Y, Log2Z);
		}
		inline s_vec3i		do_get_slog() const {
			return s_vec3i(sLog2X, sLog2Y, sLog2Z);
		}
		inline s_vec3i		do_get_child_slog() const {
			return s_vec3i(1, 1, 1);
		}
		inline s_vertex		get_pos_from_offset(unsigned int i) const
		{
			s_vertex	v;

			uint32_t	x_of = (i >> (Log2Y + Log2Z));
			uint32_t	y_of = (i >> (Log2Z)) & ((1 << Log2Y) - 1);
			uint32_t	z_of = (i) & ((1 << Log2Z) - 1);
			v.pos.x = (float)m_x + x_of;
			v.pos.y = (float)m_y + y_of;
			v.pos.z = (float)m_z + z_of;
			return (v);
		}

		Value				m_leaf_data[sSize];	//direct access table

		std::bitset<sSize>	m_value_mask;		//active states
		//[BitMask<sSize>	mInsideMask];		//optional for LS
		//uint64_t			mFlags;				//64 bit flags
		int32_t				m_x, m_y, m_z;	//origin of node
};

template <class Value, int Log2X, int Log2Y, int Log2Z>
Leaf_node<Value, Log2X, Log2Y, Log2Z>
	::Leaf_node(int32_t x, int32_t y, int32_t z): m_x(x), m_y(y), m_z(z)
{
	m_value_mask.reset();
}

template <class Value, int Log2X, int Log2Y, int Log2Z>
void	Leaf_node<Value, Log2X, Log2Y, Log2Z>
		::do_set_vox(Value value, int32_t x, int32_t y, int32_t z)
{
	unsigned int	leaf_offset = ((x & (1 << sLog2X)-1) << (Log2Y + Log2Z))
		+ ((y & (1 << sLog2Y)-1) << Log2Z) + (z & (1 << sLog2Z) - 1);

	m_leaf_data[leaf_offset] = value;
	m_value_mask.set(leaf_offset);
}

template <class Value, int Log2X, int Log2Y, int Log2Z>
Value	Leaf_node<Value, Log2X, Log2Y, Log2Z>
		::do_get_vox(int32_t x, int32_t y, int32_t z) const
{
	unsigned int	leaf_offset = ((x & (1 << sLog2X)-1) << (Log2Y + Log2Z))
		+ ((y & (1 << sLog2Y)-1) << Log2Z) + (z & (1 << sLog2Z) - 1);

	if (m_value_mask[leaf_offset])
		return (m_leaf_data[leaf_offset]);
	return (0);
}

template <class Value, int Log2X, int Log2Y, int Log2Z>
const Node<Value>	*Leaf_node<Value, Log2X, Log2Y, Log2Z>
		::do_get_interresting_node(s_vec3i v, Value &value) const
{
	unsigned int	leaf_offset = ((v.x & (1 << sLog2X)-1) << (Log2Y + Log2Z))
		+ ((v.y & (1 << sLog2Y)-1) << Log2Z) + (v.z & (1 << sLog2Z) - 1);

	if (m_value_mask[leaf_offset])
		value = m_leaf_data[leaf_offset];
	return (this);
}

template <class Value, int Log2X, int Log2Y, int Log2Z>
Value		Leaf_node<Value, Log2X, Log2Y, Log2Z>
	::pruning()
{
	Value	val;

	if (m_value_mask.all())
	{
		return (m_leaf_data[0]);
	}
	return (0);
}

template <class Value, int Log2X, int Log2Y, int Log2Z>
void		Leaf_node<Value, Log2X, Log2Y, Log2Z>
	::do_mesh(Mesh &mesh) const
{
	uint32_t	v_idx[8];

	for (int i = 0; i < sSize; i++)
	{
		if (m_value_mask[i]) 
		{
			s_vertex	v = get_pos_from_offset(i);

			v.color = glm::vec3(1.0f, 1.0, 0.0);
			v_idx[0] = mesh.add_vertex_with_basic_index(v);
			v.pos.x += 1;
			v_idx[1] = mesh.add_vertex_with_basic_index(v);
			v.pos.z += 1;
			v_idx[2] = mesh.add_vertex_with_basic_index(v);
			v.pos.x -= 1;
			v_idx[3] = mesh.add_vertex_with_basic_index(v);
			mesh.add_index(v_idx[0]);
			for (int a = 0; a < 4; a++)
			{
				v = mesh.vertex_buffer[v_idx[a]];
				v.pos.y += 1;
				v.color = glm::vec3(0, 0, 1.0);
				v_idx[4 + a] = mesh.add_vertex_with_basic_index(v);
			}
			mesh.add_index(v_idx[7]);

			mesh.add_index(v_idx[0]);
			mesh.add_index(v_idx[1]);
			mesh.add_index(v_idx[5]);
			mesh.add_index(v_idx[4]);
			mesh.add_index(v_idx[0]);

			mesh.add_index(v_idx[1]);
			mesh.add_index(v_idx[2]);
			mesh.add_index(v_idx[6]);
			mesh.add_index(v_idx[5]);
			mesh.add_index(v_idx[1]);

			mesh.add_index(v_idx[2]);
			mesh.add_index(v_idx[3]);
			mesh.add_index(v_idx[7]);
			mesh.add_index(v_idx[6]);
			mesh.add_index(v_idx[2]);
			
			mesh.add_index(v_idx[3]);
			mesh.add_index(v_idx[0]);
			mesh.add_index(v_idx[4]);
			mesh.add_index(v_idx[7]);
			mesh.add_index(v_idx[3]);
		}
	}
}
