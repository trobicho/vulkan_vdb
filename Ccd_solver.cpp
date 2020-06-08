/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ccd_solver.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/06 14:23:44 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/08 18:32:39 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ccd_solver.h"
#include <glm/gtx/transform.hpp>

#define DIST_MIN	0.05

void	Ccd_solver::ccd_solve(std::vector<glm::mat4> &bones
			, std::vector<glm::vec3> &bones_pos, glm::vec3 target)
{
	std::vector<glm::vec3>	pos_save = bones_pos;
	for (int i = 0; i < bones.size(); i++)
	{
		bones[i] = glm::translate(glm::mat4(1.0f), bones_pos[i]);
	}
	for (int a = 0; a < 8; a++)
	{
		for (int i = bones.size() - 1; i >= 0; i--)
		{
			if (glm::length(bones_pos.back() - target) < DIST_MIN)
				break;
			else if (glm::length(bones_pos[i] - target) < DIST_MIN)
				break;
			calc_matrix(bones, bones_pos, i, target);
		}
	}
	for (int i = 0; i < bones.size(); i++)
	{
		bones[i] = glm::translate(bones[i], -pos_save[i]);
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
	if (id != 0)
		r.y = 0.f;
	r.x = 0.f;
	r = glm::normalize(r);
	if (r != r)
		return ;

	glm::mat4	temp = glm::rotate(glm::mat4(1.f), angle, r);
	bones[id] *= temp;
	temp = glm::translate(glm::mat4(1.f), bones_pos[id]) * temp;
	temp = glm::translate(temp, -bones_pos[id]);
	for (int i = id + 1; i < bones.size(); i++)
	{
		bones_pos[i] = temp * glm::vec4(bones_pos[i], 1.0f);
		bones[i] = temp * bones[i];
	}
	bones_pos.back() = temp * glm::vec4(bones_pos.back(), 1.0f);
	if (bones_pos[id].x != bones_pos[id].x)
	{
		r += r;
	}
}

/*
void	Ccd_solver::calc_matrix(std::vector<glm::mat4> &bones
			, std::vector<glm::vec3> &bones_pos, int id, glm::vec3 target)
{
	float		angle;
	glm::vec3	cur_back = bones_pos.back();
	if (id > 0)
		cur_back = glm::vec4(cur_back, 1.f) * bones[id - 1];
	glm::vec3	v1 = glm::normalize(cur_back - bones_pos[id]);
	glm::vec3	v2 = glm::normalize(target - bones_pos[id]);

	angle = glm::acos(glm::dot(v1, v2));
	glm::vec3	r = glm::cross(v1, v2);
	r = glm::normalize(r);

	glm::vec3	invert_bones = glm::vec3(glm::vec4(bones_pos[id], 1.0f)
					 * (id > 0 ? glm::inverse(bones[id - 1]) : glm::mat4(1.f)));
	bones[id] = glm::translate(bones[id], invert_bones);
	bones[id] = glm::rotate(bones[id], angle, r);
	bones[id] = glm::translate(bones[id], -invert_bones);
	if (id > 0)
	{
		bones[id] = bones[id - 1] * bones[id];
	}
	bones_pos[id + 1] = glm::vec4(bones_pos[id + 1], 1.f) * bones[id];
}
*/

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
