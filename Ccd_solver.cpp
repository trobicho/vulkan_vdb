/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ccd_solver.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/06 14:23:44 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/22 15:41:52 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ccd_solver.h"
#include <glm/gtx/transform.hpp>

void	Ccd_solver::ccd_solve(std::vector<glm::mat4> &bones
			, std::vector<glm::vec3> &bones_pos, glm::vec3 target)
{
	for (int i = 0; i < bones.size(); i++)
	{
		calc_matrix(bones, bones_pos, i, target);
	}
}

void	Ccd_solver::calc_matrix(std::vector<glm::mat4> &bones
			, std::vector<glm::vec3> &bones_pos, int id, glm::vec3 target)
{
	float		angle;
	glm::vec3	v1 = glm::normalize(bones_pos.back() - bones_pos[id]);
	glm::vec3	v2 = glm::normalize(target - bones_pos[id]);

	angle = glm::acos(glm::dot(v1, v2));
	glm::vec3	r = glm::cross(v1, v2);

	bones[id] = glm::translate(glm::mat4(1.0f), bones_pos[id]);
	if (id > 0)
		bones[id] *= bones[id - 1];
	bones[id] = glm::rotate(bones[id], angle, r);

	for (int i = id; i < bones.size(); i++)
	{
		bones_pos[i + 1] = glm::vec4(bones_pos[i + 1], 0.f) * bones[id];
	}
	bones[id] = glm::translate(bones[id]
				, glm::vec3(-bones_pos[id].x
					, -bones_pos[id].y, -bones_pos[id].z));
}
