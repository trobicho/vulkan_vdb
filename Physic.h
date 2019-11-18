/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Physic.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 23:36:18 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/18 01:47:06 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Player.h"
#include "Vdb_test.h"

class	Physic
{
	public:
		Physic(Vdb_test &vdb): m_vdb(vdb){};

		void	apply_physic_to_player(Player &player);

	private:
		Vdb_test	&m_vdb;
};
