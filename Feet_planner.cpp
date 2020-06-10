/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Feet_planner.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/09 20:46:34 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/09 21:10:56 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Feet_planner.h"

glm::vec3	Feet_planner::best_foot_relative_pos(Character &mob
				, int foot_id)
{
	const s_foot_info	&foot_inf = mob.get_foot_info_ref(foot_id);
	glm::vec3			target = mob.get_center();
	glm::vec3			v = foot_inf.right_side ? glm::vec3(1.f, 0.f, 0.f)
		: glm::vec3(-1.f, 0.f, 0.f);
	v = glm::vec4(v, 1.f) * glm::rotate(glm::mat4(1.0f)
			, foot_inf.base_angle_torso
			, glm::vec3(0.f, 1.f, 0.f));
	target += v * foot_inf.base_dist_torso;
	target.y = foot_inf.base_height;
	target *= mob.get_scale();
	return (target);
}
