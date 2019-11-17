/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Player.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 17:04:20 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/17 18:24:08 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "My_vulkan.h"
#include "Vdb_test.h"
#include "Mesh.h"

#define		M_STATE_FORWARD		(1)
#define		M_STATE_BACKWARD	(1 << 1)
#define		M_STATE_RIGHT		(1 << 2)
#define		M_STATE_LEFT		(1 << 3) 

enum e_mode
{
	DEV_MODE,
	NORMAL_MODE,
};

struct	s_hitbox
{
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

		inline void		set_state(uint32_t state) {m_move_state |= state;}
		inline void		unset_state(uint32_t state) {m_move_state &= (~state);}
		inline void		reset_state() {m_move_state = 0;}
		inline void		set_mode(e_mode mode) {m_mode = mode;}
		inline Camera&	get_cam_ref() {return (m_cam);}
		inline void		update_ubo(){m_cam.update_ubo();}

		void			move();

	private:
		uint32_t	m_move_state = 0;
		Camera		m_cam;
		e_mode		m_mode = DEV_MODE;
		glm::vec3	m_pos;
		s_hitbox	hitbox;
};
