/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ccd_solver.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/06 14:23:44 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/27 18:10:24 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ccd_solver.h"
#include <glm/gtx/transform.hpp>

void	Ccd_solver::ccd_solve(std::vector<glm::mat4> &bones
			, std::vector<glm::vec3> &bones_pos, glm::vec3 target)
{
	for (int i = 0; i < bones.size(); i++)
	{
		bones[i] = glm::mat4(1.0f);
	}
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
	r = glm::normalize(r);

	glm::vec3	invert_bones = glm::vec3(glm::vec4(bones_pos[id], 1.0f)
					 * (id > 0 ? glm::inverse(bones[id - 1]) : glm::mat4(1.f)));
	bones[id] = glm::translate(bones[id], invert_bones);
	bones[id] = glm::rotate(bones[id], angle, r);
	if (id > 0)
	{
		bones[id] *= bones[id - 1];
	}
	bones[id] = glm::translate(bones[id]
				, glm::vec3(-bones_pos[id].x
					, -bones_pos[id].y, -bones_pos[id].z));
	bones_pos[id + 1] = glm::vec4(bones_pos[id + 1], 1.f) * bones[id];
}

void	Ccd_solver::apply_matrix(std::vector<glm::mat4> &bones
			, std::vector<glm::vec3> &bones_pos, int id)
{
/*
	if (id >= bones.size())
		return ;
	bones[id] = glm::translate(bones[id], bones_pos[id]) * bones[id - 1];
	bones[id] = glm::translate(bones[id]
				, glm::vec3(-bones_pos[id].x
					, -bones_pos[id].y, -bones_pos[id].z));
	bones_pos[id] = glm::vec4(bones_pos[i + 1], 1.f) * bones[id];
*/
}
