/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/19 16:15:15 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/27 15:29:16 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include "Moore_accessor.h"
#include "Mesh.h"
#include "Physic_entity.h"

#define		E_STATE_FALLING		(1 << 2) 

using	Leaf_enemy = Leaf_node<uint32_t, 3, 2, 2>;
using	Internal_enemy = Internal_node<uint32_t, Leaf_enemy, 4>;

class Spider : public Physic_entity
{
	public:
		Spider();
		~Spider(){};

		void		bones_test();
		void		generate();
		void		add_box(s_vbox box, uint32_t value);
					// not here namespace toolbox::
		inline Mesh<s_vertex_bones>
					&get_mesh_ref(){return (m_mesh);}
		inline std::vector<glm::mat4>
					&get_bones_ref(){return (m_bones);}
		void		set_target_world(glm::vec3 pos) {m_target_world = pos;}
		void		move(const Vdb_test &world);

		glm::vec3	get_pos() const {return (m_pos);}
		void		set_pos(glm::vec3 pos) {m_pos = pos;}
		bool		check_ground(const Vdb_test &world);
		s_hitbox	get_hitbox() const {return (m_hitbox);}
		glm::vec3&	get_velocity_vec_ref() {return (m_speed_vec);}

	private:
		void					one_leg_move(int leg_id, glm::vec3 target);

		uint32_t				m_state = 0;
		std::vector<glm::vec3>	m_target_leg;
		std::vector<glm::mat4>	m_bones;
		std::vector<glm::vec3>	m_bones_pos;
		Internal_enemy			m_root_node = Internal_enemy(0, 0, 0);
		Vdb_test				m_vdb = Vdb_test(m_root_node);
		Moore_accessor			m_moore_access = Moore_accessor(m_vdb);
		Mesh<s_vertex_bones>	m_mesh;
		glm::vec3				m_pos = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3				m_speed_vec = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3				m_center;
		s_hitbox				m_hitbox;
		glm::vec3				m_target_world;
};
