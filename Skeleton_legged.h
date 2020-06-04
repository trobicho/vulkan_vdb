/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skeleton_legged.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/02 13:38:47 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/03 13:07:41 by trobicho         ###   ########.fr       */
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
		Skeleton_legged(glm::vec3 torso_center);
		~Skeleton_legged(){};

	private:
		std::vector<glm::mat4>	m_bones;
		std::vector<glm::vec3>	m_bones_pos;
		int						m_leg_offset = 1;
		int						m_nb_leg = 0;
};
