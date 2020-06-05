/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character_controller.h                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/02 13:25:14 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/05 09:53:53 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Character.h"
#include "Vdb_test.h"

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
		void	swing_test(Character &mob, int feet_id);

		const Vdb_test	&m_world;
};
