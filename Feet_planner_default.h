/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Feet_planner_default.h                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/09 20:54:19 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/10 12:31:02 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Feet_planner.h"

class	Feet_planner_default: public Feet_planner
{
	public:
		Feet_planner_default(){};
		~Feet_planner_default(){};
		
		void	calc_next_foot_target(Character &mob
					, const Vdb_test &world, int foot_id);
	private:
		bool	base_target_refined_world(Character &mob
					, const Vdb_test &world, int foot_id, glm::vec3 &target);
		int		get_floor_column(const Vdb_test &world, s_vec3i &vox);
};
