/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Feet_planner.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/09 20:04:35 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/09 21:15:59 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include "Character.h"
#include <glm/gtx/transform.hpp>

class	Feet_planner
{
	public:
		Feet_planner(){};
		~Feet_planner(){};

		virtual void		calc_next_foot_target(Character &mob
								, const Vdb_test &world, int foot_id) = 0;
		static glm::vec3	best_foot_relative_pos(Character &mob
								, int foot_id);
};
