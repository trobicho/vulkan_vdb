/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character_controller.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/02 13:33:38 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/05 14:28:16 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character_controller.h"

void	Character_controller::update(Character &mob)
{
	if (mob.get_state() & CHAR_STATE_FALLING)
		return ;
	if (mob.get_state() & CHAR_STATE_WALKING || true)
	{
		mob.step();
		const auto &feet_info = mob.get_feet_info_ref();
		for (int i = 0; i < feet_info.size(); i++)
		{
			switch (feet_info[i].state)
			{
				case f_stance:
					break;
				case f_swing:
					break;
				case f_new_swing:
					swing_test(mob, i);
					break;
			}
		}
	}
}

void	Character_controller::swing_test(Character &mob, int foot_id)
{
	glm::vec3	dir = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3	target = mob.get_foot_target_world(foot_id);

	target += dir * 0.2f;
	mob.foot_to_target_world(foot_id, target);
}
