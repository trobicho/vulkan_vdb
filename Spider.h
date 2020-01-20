/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/19 16:15:15 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/25 16:59:24 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include "Moore_accessor.h"
#include "Mesh.h"

using	Leaf_enemy = Leaf_node<uint32_t, 3, 2, 2>;
using	Internal_enemy = Internal_node<uint32_t, Leaf_enemy, 4>;

class Spider
{
	public:
		Spider();
		~Spider(){};

		void		generate();
		void		add_box(s_vbox box, uint32_t value);
					// not here namespace toolbox::
		inline Mesh<s_vertex_bones>
					&get_mesh_ref(){return (m_mesh);}

	private:
		std::vector<glm::mat4>	m_bones;
		Internal_enemy			m_root_node = Internal_enemy(0, 0, 0);
		Vdb_test				m_vdb = Vdb_test(m_root_node);
		Moore_accessor			m_moore_access = Moore_accessor(m_vdb);
		Mesh<s_vertex_bones>	m_mesh;
};
