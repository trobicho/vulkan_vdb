/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character_controller.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/02 13:33:38 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/08 19:45:04 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character_controller.h"
#include <glm/gtx/transform.hpp>

void		Character_controller::update(Character &mob)
{
	if (mob.get_state() & CHAR_STATE_FALLING)
		return ;
	if (mob.get_state() & CHAR_STATE_WALKING || true)
	{
		mob.move(glm::vec3(0.f, 0.f, 1.f) * 0.05f);
		mob.step();
		const auto &feet_info = mob.get_feet_info_ref();
		for (int i = 0; i < feet_info.size(); i++)
		{
			switch (feet_info[i].state)
			{
				case f_stance:
					break;
				case f_swing:
					swing_phase(mob, i);
					break;
				case f_new_swing:
					new_swing_phase(mob, i);
					break;
			}
		}
		mob.ik_all();
	}
}

void		Character_controller::swing_phase(Character &mob, int foot_id)
{
	const s_foot_info	&foot_info = mob.get_foot_info_ref(foot_id);
	const s_feet_gate	&gate_info = mob.get_feet_gate_ref();
	float				time_last = ((gate_info.cur - 1) - foot_info.begin_gate)
	 						/ (float)(foot_info.end_gate - foot_info.begin_gate);
	float				time = (gate_info.cur - foot_info.begin_gate)
	 						/ (float)(foot_info.end_gate - foot_info.begin_gate);
	glm::vec3			&cur_target = mob.get_foot_target_world_ref(foot_id);
	if (time > 0.95)
	{
		cur_target = mob.get_foot_next_target_world(foot_id);
		return ;
	}
	glm::vec3			vec = glm::vec3(mob.get_foot_next_target_world(foot_id)
								- cur_target);
	float				len = glm::length(vec) / (1.0f - time_last);
	cur_target += glm::normalize(vec) * len * (time - time_last);
}

void		Character_controller::new_swing_phase(Character &mob, int foot_id)
{
	glm::vec3	dir = glm::vec3(0.f, 0.f, 1.f);
	//glm::vec3	target = mob.get_foot_target_world(foot_id);
	glm::vec3	target;

	target = best_foot_relative_pos(mob, mob.get_foot_info_ref(foot_id))
			+ glm::vec3(0.f, 0.f, 1.f) * 0.4f + mob.get_pos();
	mob.set_foot_next_target_world(foot_id, target);
}

glm::vec3	Character_controller::best_foot_relative_pos(Character &mob
						, s_foot_info &foot_inf)
{
	glm::vec3	target = mob.get_center();
	glm::vec3	v = foot_inf.right_side ? glm::vec3(1.f, 0.f, 0.f)
						: glm::vec3(-1.f, 0.f, 0.f);
	v = glm::vec4(v, 1.f) * glm::rotate(glm::mat4(1.0f)
			, foot_inf.base_angle_torso
			, glm::vec3(0.f, 1.f, 0.f));
	target += v * foot_inf.base_dist_torso;
	target.y = foot_inf.base_height;
	target *= mob.get_scale();
	return (target);
}
