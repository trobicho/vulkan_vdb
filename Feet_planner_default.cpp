/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Feet_planner_default.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/09 20:23:28 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/11 11:09:51 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Feet_planner_default.h"

void	Feet_planner_default::calc_next_foot_target(Character &mob
			, const Vdb_test &world, int foot_id)
{
	float		desired_len = 0.4f;
	glm::vec3	vec = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3	base_target = best_foot_relative_pos(mob, foot_id)
					+ mob.get_pos() + vec * desired_len;


	base_target_refined_world(mob, world, foot_id, base_target);
	mob.set_foot_next_target_world(foot_id, base_target);
	auto &foot_inf = mob.get_foot_info_ref(foot_id);
	foot_inf.vector_to_next_target = base_target 
		- mob.get_foot_target_world_ref(foot_id);
}

bool	Feet_planner_default::base_target_refined_world(Character &mob
			, const Vdb_test &world, int foot_id, glm::vec3 &target)
{
	s_vec3i		vox((int)target.x, (int)target.y, (int)target.z);
	int			floor = get_floor_column(world, vox);
	target.y = vox.y + 1.0f;
	return (floor ? true : false);
}

int		Feet_planner_default::get_floor_column(const Vdb_test &world
			, s_vec3i &vox)
{
	int	found = 0;
	int	limit = 4;

	if ((found = world.get_vox(vox)))
	{
		for (int i = 0; i < limit && (found = world.get_vox(vox)); i++)
		{
			vox.y++;
		}
		if (found == 0)
		{
			vox.y -= 1;
			return (world.get_vox(s_vec3i(vox.x, vox.y - 1, vox.z)));
		}
	}
	else
	{
		for (int i = 0; i < limit && (found = world.get_vox(vox)) == 0; i++)
		{
			vox.y--;
		}
		if (found != 0)
		{
			return (found);
		}
	}
	return (0);
}
