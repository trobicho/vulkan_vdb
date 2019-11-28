/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Player.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 17:19:36 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/28 11:35:07 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Player.h"

Player::Player(glm::vec3 pos): m_pos(pos)
{
	m_cam.ubo.model = glm::rotate(glm::mat4(1.0f)
						, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_cam.pos = m_pos;
	m_cam.pos.y += 0.4 * m_hitbox.h;
	m_cam.dir = glm::vec3(0.0f, 0.0f, 1.0f);
	m_cam.up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_cam.right = glm::vec3(1.0f, 0.0f, 0.0f);
}

void	Player::move()
{
	if (m_mode == DEV_MODE)
		move_dev();
	else
	{
		move_normal();
		m_cam.pos = m_pos;
		m_cam.pos.y += 0.4 * m_hitbox.h;
	}
}

void	Player::move_normal() //TMP
{
	float	accel_y = m_accel_vec.y;
	m_accel_vec.x = 0.0f;
	m_accel_vec.z = 0.0f;
	m_dir = m_cam.dir;
	m_dir.y = 0;
	m_dir = glm::normalize(m_dir);
	if (m_state & P_STATE_FORWARD)
		m_accel_vec = m_dir * 4.0f;
	if (m_state & P_STATE_BACKWARD)
		m_accel_vec = m_dir * -4.0f;
	if (m_state & P_STATE_RIGHT)
		m_accel_vec = m_cam.right * -4.0f;
	if (m_state & P_STATE_LEFT)
		m_accel_vec = m_cam.right * 4.0f;
	m_accel_vec.y = accel_y;
}

void	Player::move_dev()
{
	if (m_state & P_STATE_FORWARD)
		m_cam.pos += m_cam.dir * 2.0f;
	if (m_state & P_STATE_BACKWARD)
		m_cam.pos -= m_cam.dir * 2.0f;
	if (m_state & P_STATE_RIGHT)
		m_cam.pos -= m_cam.right * 2.0f;
	if (m_state & P_STATE_LEFT)
		m_cam.pos += m_cam.right * 2.0f;
}

void	Player::apply_force(float t) // get/set_time
{
	glm::vec3	v = m_speed_vec;

	if (!is_falling() && v.y < 0.f)
		v.y = 0;

	m_pos += m_speed_vec * t;
}

void	Player::touch_ground()
{
	unset_state(P_STATE_FALLING);

	if (m_accel_vec.y < 0.0f)
		m_accel_vec.y = 0.0f;
	if (m_speed_vec.y < 0.0f)
		m_speed_vec.y = 0.0f;
}

void	Player::collide_eject(glm::vec3 eject_vector, float d)
{
	m_pos += eject_vector * d;
	if (eject_vector.x < 0 && m_speed_vec.x > 0)
		m_speed_vec.x = 0;
	else if (eject_vector.x > 0 && m_speed_vec.x < 0)
		m_speed_vec.x = 0;
	if (eject_vector.y < 0 && m_speed_vec.y > 0)
		m_speed_vec.y = 0;
	else if (eject_vector.y > 0 && m_speed_vec.y < 0)
		m_speed_vec.y = 0;
	if (eject_vector.z < 0 && m_speed_vec.z > 0)
		m_speed_vec.z = 0;
	else if (eject_vector.z > 0 && m_speed_vec.z < 0)
		m_speed_vec.z = 0;
}

void	Player::alternate_mode()
{
	if (m_mode == DEV_MODE)
		m_mode = NORMAL_MODE;
	else
		m_mode = DEV_MODE;
	m_state &= (1 << 5) - 1;
	m_accel_vec = glm::vec3(0.f, 0.f, 0.f);
	m_speed_vec = glm::vec3(0.f, 0.f, 0.f);
}
