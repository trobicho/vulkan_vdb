/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Physic.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 23:47:42 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/19 02:28:12 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Physic.h"
#include <iostream>
#include <cmath>

static float	get_vox_dist(glm::vec3 pos, s_vec3i	vox
					, bool b_x, bool b_y, bool b_z)
{
	float	d = 0.0f;
	int		int_x = (int)pos.x;
	int		int_y = (int)pos.y;
	int		int_z = (int)pos.z;

	if (b_x)
	{
		if (int_x < vox.x)
			d += vox.x - pos.x;
		else if (int_x > vox.x)
			d += pos.x - (vox.x + 1);
	}
	if (b_y)
	{
		if (int_y < vox.y)
			d += vox.y - pos.y;
		else if (int_y > vox.y)
			d += pos.y - (vox.y + 1);
	}
	if (b_z)
	{
		if (int_z < vox.z)
			d += vox.z - pos.z;
		else if (int_z > vox.z)
			d += pos.z - (vox.z + 1);
	}
	return (d);
}

static float	friction_calc(float speed, float friction, float t)
{
	return (speed * pow(1.0 - friction, t));
}

Physic::Physic(Vdb_test &vdb): m_vdb(vdb)
		, m_start_time(std::chrono::high_resolution_clock::now())
{
	m_last_time = m_start_time;
}

void	Physic::apply_physic_to_player(Player &player)
{
	float 		time;
	auto		current_time = std::chrono::high_resolution_clock::now();

	time = std::chrono::duration<float
			, std::chrono::seconds::period>(current_time - m_last_time).count();

	glm::vec3	p_pos = player.get_pos();
	glm::vec3&	p_accel_vec = player.get_accel_vec_ref();
	glm::vec3&	p_speed_vec = player.get_speed_vec_ref();
	float		friction;
	s_vec3i		vox_p((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);

	if (m_vdb.get_vox(vox_p))
	{
		std::cout << "PANIC !!" << std::endl;
		player.unset_state((uint32_t)(1 << 31) - 1);
		player.set_state(P_STATE_PANIC);
		return ;
	}

	friction = check_ground(player);
	if (player.is_falling())
		p_accel_vec.y = -18.8f;

	//std::cout << "time = " << time << std::endl;
	apply_friction(p_speed_vec, p_accel_vec, friction, time);
	player.apply_force(time);
	m_last_time = current_time;
}

float	Physic::check_ground(Player &player)
{
	glm::vec3	p_pos = player.get_pos();
	s_hitbox	hitbox = player.get_hitbox();
	float		friction = 0.3; 
	s_vec3i		vox_p((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);
	s_vec3i		vox = vox_p;
	float		d_eject;
	int			found = 0;

	float	foot_h = (p_pos.y - hitbox.h / 2.0);
	vox.y = (int)(foot_h - 0.05);
	if (vox.y != vox_p.y)
	{
		vox.x -= 1;
		vox.z -= 1;
		for (int i = 0; i < 9; i++)
		{
			if (i && i % 3 == 0)
			{
				vox.z++;
				vox.x = vox_p.x - 1;
			}
			if (get_vox_dist(p_pos, vox, true, false, true) < hitbox.w)
				if ((found = m_vdb.get_vox(vox)))
					break;
			vox.x++;
		}
	}
	if (found)
	{
		float d = (vox.y + 1.0f) - foot_h;
		if (d > 0.05)
			player.collide_eject(glm::vec3(0.f, 1.f, 0.f), d);
		if (player.is_falling())
			player.touch_ground();
		friction = 0.8;
	}
	else
	{
		player.set_state(P_STATE_FALLING);
	}
	return (friction);
}

void	Physic::apply_friction(glm::vec3 &speed_vec
			, glm::vec3 accel_vec, float friction, float time)
{
	float	speed = length(speed_vec);
	float	accel = length(accel_vec);
	if (accel > 0.01)
	{
		speed_vec += (glm::normalize(accel_vec) * time)
			* (accel - friction * speed);
	}
	else if (speed > 0.0)
	{
		float scalar = friction_calc(speed, friction, time);
		if (scalar > 0.0)
			speed_vec = glm::normalize(speed_vec) * scalar;
	}
}
