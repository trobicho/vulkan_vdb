/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Physic.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 23:47:42 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/18 03:21:55 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Physic.h"
#include <iostream>

void	Physic::apply_physic_to_player(Player &player)
{
	glm::vec3	p_pos = player.get_pos();
	s_hitbox	hitbox = player.get_hitbox();
	s_vec3i		vox_p((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);
	s_vec3i		vox = vox_p;
	double		d_eject;

	if (m_vdb.get_vox(vox_p))
	{
		std::cout << "PANIC !!" << std::endl;
		player.unset_state((uint32_t)(1 << 31) - 1);
		player.set_state(P_STATE_PANIC);
		return ;
	}

	vox.y = (int)(p_pos.y - hitbox.h / 2.0);
	if (vox.y != vox_p.y && m_vdb.get_vox(vox))
	{
		double d = p_pos.y - vox.y;
		if (d > 0.001)
			player.collide_eject(glm::vec3(0.f, 1.f, 0.f), d);
		player.unset_state(P_STATE_FALLING);
	}
	else
		player.set_state(P_STATE_FALLING);
}
