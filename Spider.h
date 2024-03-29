/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/19 16:15:15 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/08 18:01:10 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include "Moore_accessor.h"
#include "Mesh.h"
#include "Physic_entity.h"
#include "Character.h"

using	Leaf_enemy = Leaf_node<uint32_t, 3, 2, 2>;
using	Internal_enemy = Internal_node<uint32_t, Leaf_enemy, 4>;

class Spider : public Physic_entity, public Character
{
	public:
		Spider(glm::vec3 pos);
		~Spider(){};

		void		generate();
		void		add_box(s_vbox box, uint32_t value);
					// not here namespace toolbox::
		inline Mesh<s_vertex_bones>
					&get_mesh_ref(){return (m_mesh);}
		inline std::vector<glm::mat4>
					&get_bones_ref(){return (m_bones);}
		void		set_target_world(glm::vec3 pos) {m_target_world = pos;}

		glm::vec3	get_pos() const {return (m_pos);}
		void		set_pos(glm::vec3 pos) {m_pos = pos;}
		bool		check_ground(const Vdb_test &world);
		s_hitbox	get_hitbox() const {return (m_hitbox);}
		glm::vec3&	get_velocity_vec_ref() {return (m_speed_vec);}

		int			get_state(){return (m_state);}
		void		foot_to_target_world(int foot_id, glm::vec3 target_world);
		float		get_scale() const {return (0.1f);}
		glm::vec3	get_center() const {return (m_center);}
		void		foot_to_target_relative(int leg_id, glm::vec3 target);
		void		ik_all();
		void		move(glm::vec3 v) {m_pos += v;}

	private:

		uint32_t				m_state = 0;
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
