/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Physic.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 23:36:18 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/25 15:36:13 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Physic_entity.h"
#include "Player.h"
#include "Vdb_test.h"
#include <chrono>

class	Physic
{
	public:
		Physic(const Vdb_test &vdb);

		void	apply_physic(Physic_entity &entity);
		void	apply_physic_to_player(Player &player);
		float	check_ground(Player &player);
		void	check_collision(Player &player);
		void	resync_time() {m_last_time
					= std::chrono::high_resolution_clock::now();}

	private:
		void	apply_friction(glm::vec3 &speed_vec
					, glm::vec3 accel_vec, float friction, float time);

		const Vdb_test	&m_vdb;
		const std::chrono::time_point<std::chrono::high_resolution_clock
						, std::chrono::duration<long int
							, std::ratio<1, 1000000000>>>
					m_start_time;
		std::chrono::time_point<std::chrono::high_resolution_clock
						, std::chrono::duration<long int
							, std::ratio<1, 1000000000>>>
					m_last_time;
};
