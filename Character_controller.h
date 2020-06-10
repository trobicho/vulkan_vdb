/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character_controller.h                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/02 13:25:14 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/09 21:13:14 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Character.h"
#include "Vdb_test.h"
#include "Feet_planner_default.h"

struct	s_character_info
{
	float				speed;
	float				step_height;
	std::vector<float>	feet_spacing;
};

class	Character_controller
{
	public:
		Character_controller(const Vdb_test &world): m_world(world){};
		~Character_controller(){};

		void	update(Character &mob);

	private:
		void		swing_phase(Character &mob, int feet_id);
		void		new_swing_phase(Character &mob, int feet_id);

		const Vdb_test			&m_world;
		Feet_planner_default	m_feet_planner;
};
