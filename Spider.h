/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/19 16:15:15 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/24 14:09:31 by trobicho         ###   ########.fr       */
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
		Spider(const Vdb_test &world);
		~Spider(){};

		void		bones_test();
		void		generate();
		void		set_pos(glm::vec3 pos) {m_pos = pos;}
		glm::vec3	get_pos() {return (m_pos);}
		void		add_box(s_vbox box, uint32_t value);
					// not here namespace toolbox::
		inline Mesh<s_vertex_bones>
					&get_mesh_ref(){return (m_mesh);}
		inline std::vector<glm::mat4>
					&get_bones_ref(){return (m_bones);}

	private:
		void					one_leg_move(int leg_id, glm::vec3 target);

		std::vector<glm::vec3>	m_target_leg;
		std::vector<glm::mat4>	m_bones;
		std::vector<glm::vec3>	m_bones_pos;
		Internal_enemy			m_root_node = Internal_enemy(0, 0, 0);
		Vdb_test				m_vdb = Vdb_test(m_root_node);
		const Vdb_test			&m_world;
		Moore_accessor			m_moore_access = Moore_accessor(m_vdb);
		Mesh<s_vertex_bones>	m_mesh;
		glm::vec3				m_pos = glm::vec3(0.f, 0.f, 0.f);
};
