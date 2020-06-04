/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character_controller.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/02 13:33:38 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/04 08:24:08 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character_controller.h"

void	Character_controller::update(Character &mob)
{
	if (mob.get_state() & CHAR_STATE_FALLING)
		return ;
	mob.step();
	const auto &feet_info = mob.get_feet_info_ref();
	if (mob.get_state() & CHAR_STATE_WALKING)
	{
		for (int i = 0; i < feet_info.size(); i++)
		{
			switch (feet_info[i].state)
			{
				case f_stance:
					break;
				case f_swing:
					break;
				case f_new_swing:
					break;
			}
		}
	}
}
