/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Internal_node.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 20:38:22 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/15 03:28:25 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <bitset>
#include <iostream>
#include "Node.h"

template <class Value, class Child
	, int Log2X, int Log2Y = Log2X, int Log2Z = Log2Y>
class Internal_node: public Node<Value>
{
	public:
		Internal_node(int32_t x, int32_t y, int32_t z);
		~Internal_node();

		void		do_set_vox(Value v, int32_t x, int32_t y, int32_t z);
		Value		do_get_vox(int32_t x, int32_t y, int32_t z) const;
		Value		pruning();
		const Node<Value>
					*do_get_interresting_node(s_vec3i v, Value &value) const;
		void		do_mesh(Mesh &mesh) const;

		static const int sLog2X = Log2X + Child::sLog2X,
			sLog2Y = Log2Y + Child::sLog2Y,
			sLog2Z = Log2Z + Child::sLog2Z,
			sSize = 1 << (Log2X + Log2Y + Log2Z);
	private:
		inline bool			do_is_leaf() const {
			return (false);
		}
		inline s_vec3i		do_get_log() const {
			return s_vec3i(Log2X, Log2Y, Log2Z);
		}
		inline s_vec3i		do_get_slog() const {
			return s_vec3i(sLog2X, sLog2Y, sLog2Z);
		}
		inline s_vec3i		do_get_child_slog() const {
			return s_vec3i(Child::sLog2X, Child::sLog2Y, Child::sLog2Z);
		}
		inline s_vertex		get_pos_from_offset(unsigned int i) const
		{
			s_vertex	v;

			uint32_t	x_of = (i >> (Log2Y + Log2Z));
			uint32_t	y_of = (i >> (Log2Z)) & ((1 << Log2Y) - 1);
			uint32_t	z_of = (i) & ((1 << Log2Z) - 1);
			std::cout << x_of << ", " << y_of << ", " << z_of << std::endl;
			v.pos.x = (float)m_x + (x_of << Child::sLog2X);
			v.pos.y = (float)m_y + (y_of << Child::sLog2Y);
			v.pos.z = (float)m_z + (z_of << Child::sLog2Z);
			v.tex_coord = glm::vec2(0.0f, 0.0f);
			return (v);
		}

		union u_internal_data 
		{
			Child*	child;					//child node pointer
			Value	value;					//tile value
		} m_internal_data[sSize];

		std::bitset<sSize>	m_value_mask;	//active states
		std::bitset<sSize>	m_child_mask;	//node topology
		int32_t				m_x, m_y, m_z;	//origin of node
};

template <class Value, class Child, int Log2X, int Log2Y, int Log2Z>
Internal_node<Value, Child, Log2X, Log2Y, Log2Z>
	::Internal_node(int32_t x, int32_t y, int32_t z): m_x(x), m_y(y), m_z(z)
{
	m_value_mask.reset();
	m_child_mask.reset();
	for (int i = 0; i < sSize; ++i)
	{
		m_internal_data[i].child = nullptr;
	}
}

template <class Value, class Child, int Log2X, int Log2Y, int Log2Z>
Internal_node<Value, Child, Log2X, Log2Y, Log2Z>::~Internal_node()
{
	for (int i = 0; i < sSize; ++i)
	{
		if (m_child_mask[i])
			delete m_internal_data[i].child;
		m_internal_data[i].child = nullptr;
	}
}

template <class Value, class Child, int Log2X, int Log2Y, int Log2Z>
void	Internal_node<Value, Child, Log2X, Log2Y, Log2Z>
		::do_set_vox(Value value, int32_t x, int32_t y, int32_t z)
{
	unsigned int	internal_offset =
		(((x & (1 << sLog2X) - 1) >> Child::sLog2X) << (Log2Y + Log2Z))
		+ (((y & (1 << sLog2Y) - 1) >> Child::sLog2Y) << Log2Z)
		+ ((z & (1 << sLog2Z) - 1) >> Child::sLog2Z);

	if (m_child_mask[internal_offset])
		m_internal_data[internal_offset].child->set_vox(value, x, y, z);
	else
	{
		m_internal_data[internal_offset].child = new Child(
			x % Child::sLog2X, y % Child::sLog2Y, z % Child::sLog2Z);
		m_internal_data[internal_offset].child->set_vox(value, x, y, z);
		m_child_mask.set(internal_offset);
	}
}

template <class Value, class Child, int Log2X, int Log2Y, int Log2Z>
Value	Internal_node<Value, Child, Log2X, Log2Y, Log2Z>
	::do_get_vox(int32_t x, int32_t y, int32_t z) const
{
	unsigned int	internal_offset =
		(((x & (1 << sLog2X) - 1) >> Child::sLog2X) << (Log2Y + Log2Z))
		+ (((y & (1 << sLog2Y) - 1) >> Child::sLog2Y) << Log2Z)
		+ ((z & (1 << sLog2Z) - 1) >> Child::sLog2Z);
	if (m_value_mask[internal_offset])
		return (m_internal_data[internal_offset].value);
	else if (m_child_mask[internal_offset])
		return (m_internal_data[internal_offset].child->get_vox(x, y, z));
	return (0);
}

template <class Value, class Child, int Log2X, int Log2Y, int Log2Z>
const Node<Value>	*Internal_node<Value, Child, Log2X, Log2Y, Log2Z>
	::do_get_interresting_node(s_vec3i v, Value &value) const
{
	unsigned int	internal_offset =
		(((v.x & (1 << sLog2X) - 1) >> Child::sLog2X) << (Log2Y + Log2Z))
		+ (((v.y & (1 << sLog2Y) - 1) >> Child::sLog2Y) << Log2Z)
		+ ((v.z & (1 << sLog2Z) - 1) >> Child::sLog2Z);
	if (m_value_mask[internal_offset])
	{
		value = m_internal_data[internal_offset].value;
		return (this);
	}
	else if (m_child_mask[internal_offset])
		return (m_internal_data[internal_offset].child
			->get_interresting_node(v, value));
	return (this);
}
template <class Value, class Child, int Log2X, int Log2Y, int Log2Z>
Value	Internal_node<Value, Child, Log2X, Log2Y, Log2Z>::pruning()
{
	Value	val;

	for (int i = 0; i < sSize; i++)
	{
		if (m_child_mask[i])
		{
			if ((val = m_internal_data[i].child->pruning()) != 0)
			{
				m_child_mask.set(i, false);
				m_value_mask.set(i, true);
				delete(m_internal_data[i].child);
				m_internal_data[i].value = val;
			}
		}
	}
	if (m_value_mask.all())
	{
		return (m_internal_data[0].value);
	}
	return (0);
}

template <class Value, class Child, int Log2X, int Log2Y, int Log2Z>
void	Internal_node<Value, Child, Log2X, Log2Y, Log2Z>
	::do_mesh(Mesh &mesh) const
{
	uint32_t	v_idx[8];

	for (int i = 0; i < sSize; i++)
	{
		if (m_child_mask[i])
		{
			m_internal_data[i].child->do_mesh(mesh);
		}

		else if (m_value_mask[i]) 
		{
			std::cout << "internal offset = " << i << std::endl;
			s_vertex	v = get_pos_from_offset(i);

			v_idx[0] = mesh.add_vertex_with_basic_index(v);
			std::cout << Child::sLog2X << ", " << Child::sLog2Y << ", " << Child::sLog2Z << std::endl;
			v.pos.x += (float)(1 << Child::sLog2X);
			v.tex_coord = glm::vec2(1.0f, 0.0f);
			v_idx[1] = mesh.add_vertex_with_basic_index(v);
			v.pos.z += (float)(1 << Child::sLog2Z);
			v.tex_coord = glm::vec2(1.0f, 1.0f);
			v_idx[2] = mesh.add_vertex_with_basic_index(v);
			mesh.add_index(v_idx[2]);
			v.pos.x -= (float)(1 << Child::sLog2X);
			v.tex_coord = glm::vec2(0.0f, 1.0f);
			v_idx[3] = mesh.add_vertex_with_basic_index(v);
			mesh.add_index(v_idx[0]);
			for (int a = 0; a < 4; a++)
			{
				v = mesh.vertex_buffer[v_idx[a]];
				v.pos.y += (float)(1 << Child::sLog2Y);
				v_idx[4 + a] = mesh.add_vertex_with_basic_index(v);
				if (a == 2)
					mesh.add_index(v_idx[6]);
			}
			mesh.add_index(v_idx[4]);

			int	tmp;
			mesh.add_index(v_idx[0]);
			mesh.add_index(v_idx[1]);
			v = mesh.vertex_buffer[v_idx[5]];
			v.tex_coord = glm::vec2(1.0f, 1.0f);
			tmp = mesh.add_vertex_with_basic_index(v);
			mesh.add_index(tmp);
			v = mesh.vertex_buffer[v_idx[4]];
			v.tex_coord = glm::vec2(0.0f, 1.0f);
			mesh.add_vertex_with_basic_index(v);
			mesh.add_index(v_idx[0]);
			
			v = mesh.vertex_buffer[v_idx[2]];
			v.tex_coord = glm::vec2(0.0f, 0.0f);
			mesh.add_vertex_with_basic_index(v);
			v = mesh.vertex_buffer[v_idx[1]];
			v.tex_coord = glm::vec2(1.0f, 0.0f);
			tmp = mesh.add_vertex_with_basic_index(v);
			v = mesh.vertex_buffer[v_idx[5]];
			v.tex_coord = glm::vec2(1.0f, 1.0f);
			int tmp2 = mesh.add_vertex_with_basic_index(v);
			mesh.add_index(tmp2);
			v = mesh.vertex_buffer[v_idx[6]];
			v.tex_coord = glm::vec2(1.0f, 0.0f);
			mesh.add_vertex_with_basic_index(v);
			mesh.add_index(tmp);
			
			v = mesh.vertex_buffer[v_idx[3]];
			v.tex_coord = glm::vec2(0.0f, 0.0f);
			tmp = mesh.add_vertex_with_basic_index(v);
			v = mesh.vertex_buffer[v_idx[2]];
			v.tex_coord = glm::vec2(1.0f, 0.0f);
			mesh.add_vertex_with_basic_index(v);
			mesh.add_index(v_idx[6]);
			mesh.add_index(v_idx[6]);
			mesh.add_index(v_idx[7]);
			mesh.add_index(tmp);
			
			mesh.add_index(v_idx[0]);
			v = mesh.vertex_buffer[v_idx[3]];
			v.tex_coord = glm::vec2(1.0f, 0.0f);
			mesh.add_vertex_with_basic_index(v);
			v = mesh.vertex_buffer[v_idx[7]];
			v.tex_coord = glm::vec2(1.0f, 1.0f);
			tmp = mesh.add_vertex_with_basic_index(v);
			mesh.add_index(tmp);
			v = mesh.vertex_buffer[v_idx[4]];
			v.tex_coord = glm::vec2(0.0f, 1.0f);
			mesh.add_vertex_with_basic_index(v);
			mesh.add_index(v_idx[0]);
		}
	}
}
