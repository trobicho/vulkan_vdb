/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skeleton_legged.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/02 13:38:47 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/05 09:03:23 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

class	Skeleton_legged
{
	public:
		Skeleton_legged(){};
		~Skeleton_legged(){};

		virtual void	generate() = 0;

	protected:
		std::vector<glm::mat4>	m_bones;
		std::vector<glm::vec3>	m_bones_pos;
		int						m_leg_offset = 1;
		int						m_nb_leg = 0;
};
