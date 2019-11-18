/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Player.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 17:19:36 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/18 03:12:37 by trobicho         ###   ########.fr       */
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
		if (m_state & P_STATE_FALLING)
			m_pos.y -= 0.3;
		m_cam.pos = m_pos;
		m_cam.pos.y += 0.4 * m_hitbox.h;
	}
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

void	Player::collide_eject(glm::vec3 eject_vector, float d)
{
	m_pos += eject_vector * d;
}

void	Player::alternate_mode()
{
	if (m_mode == DEV_MODE)
		m_mode = NORMAL_MODE;
	else
	{
		m_mode = DEV_MODE;
		m_state &= (1 << 5) - 1;
	}
}
