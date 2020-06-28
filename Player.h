/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Player.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 17:04:20 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/12 18:43:29 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "My_vulkan.h"
#include "Vdb_test.h"
#include "Mesh.h"
#include "Physic_entity.h"

#define		P_STATE_FORWARD		(1)
#define		P_STATE_BACKWARD	(1 << 1)
#define		P_STATE_RIGHT		(1 << 2)
#define		P_STATE_LEFT		(1 << 3) 

#define		P_STATE_FALLING		(1 << 8) 
#define		P_STATE_PANIC		(1 << 9) 

enum e_mode
{
	DEV_MODE,
	NORMAL_MODE,
};

class	Camera
{
	public:
		inline void		update_ubo()
		{
			ubo.view = glm::lookAt(pos, pos + dir, up);
		}
		glm::vec3	pos;
		glm::vec3	up;
		glm::vec3	dir;
		glm::vec3	right;
		s_ubo		ubo;
};

class	Player : public Physic_entity
{
	public:
		Player(glm::vec3 pos = glm::vec3(0, 130, 0));

		inline void			set_state(uint32_t state) {m_state |= state;}
		inline void			unset_state(uint32_t state) {m_state &= (~state);}
		inline void			reset_state() {m_state = 0;}
		inline void			set_mode(e_mode mode) {m_mode = mode;}
		inline Camera&		get_cam_ref() {return (m_cam);}
		inline void			update_ubo(){m_cam.update_ubo();}
		inline bool			is_falling() const
								{return (m_state & (P_STATE_FALLING));}
		inline bool			has_physic(){return (m_mode != DEV_MODE);}
		inline void			sync_to_cam(){m_pos = m_cam.pos;
								m_pos.y -= 0.4 * m_hitbox.h;}

		inline e_block_type	get_block_type() const {return (m_block_type);}
		inline glm::vec3	get_cam_pos() const {return (m_cam.pos);}
		inline glm::vec3&	get_speed_vec_ref() {return (m_speed_vec);}
		inline glm::vec3&	get_accel_vec_ref() {return (m_accel_vec);}
		
		inline void			set_block_type(e_block_type bl_t)
		{
			m_block_type = bl_t;
		}

		void				move();
		void				jump() {m_speed_vec.y = 8.0f;}
		void				super_jump() {m_speed_vec.y = 20.0f;}
		void				collide_eject(glm::vec3 eject_vector, float d);
		void				alternate_mode();
		void				touch_ground();
		void				apply_force(float t);
		bool				check_ground(const Vdb_test &world) {return (false);}
		
		inline glm::vec3	get_pos() const {return (m_pos);}
		inline s_hitbox		get_hitbox() const {return (m_hitbox);}
		inline glm::vec3&	get_velocity_vec_ref() {return (m_speed_vec);}
		inline void			set_pos(glm::vec3 pos) {};

	private:
		void				move_dev();
		void				move_normal();

		uint32_t		m_state = 0;
		Camera			m_cam;
		e_mode			m_mode = DEV_MODE;
		glm::vec3		m_pos;
		glm::vec3		m_dir;
		glm::vec3		m_speed_vec;
		glm::vec3		m_accel_vec;
		s_hitbox		m_hitbox = s_hitbox(0.8, 2.2);
		e_block_type	m_block_type = bl_dirt;
};
