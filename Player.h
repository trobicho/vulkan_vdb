/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Player.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 17:04:20 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/18 03:16:12 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "My_vulkan.h"
#include "Vdb_test.h"
#include "Mesh.h"

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

struct	s_hitbox
{
	s_hitbox(){};
	s_hitbox(float sw, float sh): w(sw), h(sh){};
	float	w, h;
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

class	Player
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
								{return (!(m_state ^ (~P_STATE_FALLING)));}
		inline bool			has_physic(){return (m_mode != DEV_MODE);}

		inline glm::vec3	get_pos() const {return (m_pos);}
		inline s_hitbox		get_hitbox() const {return (m_hitbox);}

		void				move();
		void				collide_eject(glm::vec3 eject_vector, float d);
		void				alternate_mode();

	private:
		void				move_dev();

		uint32_t	m_state = 0;
		Camera		m_cam;
		e_mode		m_mode = DEV_MODE;
		glm::vec3	m_pos;
		s_hitbox	m_hitbox = s_hitbox(0.4, 2.5);
};
