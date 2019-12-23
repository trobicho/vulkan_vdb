/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/19 16:15:15 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/21 21:08:49 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include "Moore_accessor.h"
#include "Mesh.h"

using	Leaf_enemy = Leaf_node<uint32_t, 2>;
using	Internal_enemy = Internal_node<uint32_t, Leaf_enemy, 4>;

class Spider
{
	public:
		Spider();
		~Spider(){};

		void		generate();
		inline Mesh	&get_mesh_ref(){return (m_mesh);}

	private:
		Internal_enemy	m_root_node = Internal_enemy(0, 0, 0);
		Vdb_test		m_vdb = Vdb_test(m_root_node);
		Moore_accessor	m_moore_access = Moore_accessor(m_vdb);
		Mesh			m_mesh;
};
