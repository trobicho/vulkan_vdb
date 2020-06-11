/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character_controller.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/02 13:33:38 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/11 11:43:33 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character_controller.h"

void		Character_controller::update(Character &mob)
{
	if (mob.get_state() & CHAR_STATE_FALLING)
	{
		for (int i = 0; i < mob.get_nb_feet(); i++)
		{
			mob.foot_to_target_relative(i
					, Feet_planner::best_foot_relative_pos(mob
						, i)
					/ mob.get_scale());
		}
		return ;
	}
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
	glm::vec3	vec = foot_info.vector_to_next_target;
	glm::vec3	center = foot_info.next_target_world - vec / 2.0f;

	float		len = glm::length(vec) / (1.0f - time_last);
	glm::mat4	temp = glm::translate(glm::mat4(1.f), center);
	glm::vec3	r = glm::cross(glm::normalize(vec), glm::vec3(0.f, 1.f, 0.f));
	temp = glm::rotate(temp, 3.141592f * (1.f - time), r);
	temp = glm::translate(temp, -center);
	cur_target = temp * glm::vec4(foot_info.next_target_world, 1.f);
}

void		Character_controller::new_swing_phase(Character &mob, int foot_id)
{
	glm::vec3	dir = glm::vec3(0.f, 0.f, 1.f);
	//glm::vec3	target = mob.get_foot_target_world(foot_id);
	glm::vec3	target;

	m_feet_planner.calc_next_foot_target(mob, m_world, foot_id);
}
