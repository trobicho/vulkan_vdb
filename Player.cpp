/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Player.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 17:19:36 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/17 19:45:25 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Player.h"

Player::Player(glm::vec3 pos): m_pos(pos)
{
	m_cam.ubo.model = glm::rotate(glm::mat4(1.0f)
						, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_cam.pos = m_pos;
	m_cam.pos.y += 0.5;
	m_cam.dir = glm::vec3(0.0f, 0.0f, 1.0f);
	m_cam.up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_cam.right = glm::vec3(1.0f, 0.0f, 0.0f);
}

void	Player::move()
{

	if (m_move_state & M_STATE_FORWARD)
		m_cam.pos += m_cam.dir * 2.0f;
	if (m_move_state & M_STATE_BACKWARD)
		m_cam.pos -= m_cam.dir * 2.0f;
	if (m_move_state & M_STATE_RIGHT)
		m_cam.pos -= m_cam.right * 2.0f;
	if (m_move_state & M_STATE_LEFT)
		m_cam.pos += m_cam.right * 2.0f;
}
