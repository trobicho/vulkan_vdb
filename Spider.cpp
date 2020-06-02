/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 22:08:47 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/01 14:07:48 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Spider.h"
#include "Ccd_solver.h"

Spider::Spider(): m_mesh(m_moore_access)
{
}

bool	Spider::check_ground(const Vdb_test &world)
{
	s_vec3i		vox((int)m_pos.x, (int)m_pos.y - 1, (int)m_pos.z);
	glm::vec3	voxi;
	int			found = 0;
	int			best_found = 0;

	for (int i = 0; i < m_target_leg.size(); i++)
	{
		voxi = m_target_leg[i] + m_pos;
		vox.x = (int)voxi.x;
		vox.y = (int)voxi.y;
		vox.z = (int)voxi.z;
		if ((found = world.get_vox(vox)))
		{
			m_pos.y += ((float)vox.y + 1.0f) - (m_pos.y + m_target_leg[i].y);
			best_found = found;
		}
		else
		{
			vox.y -= 3;
			if ((found = world.get_vox(vox)))
				best_found = found;
		}
		/*
		vox.y -= 1;
		if ((found = world.get_vox(vox)))
			break;
		*/
	}
	if (best_found)
		m_state &= (~E_STATE_FALLING);
	else
		m_state |= E_STATE_FALLING;
	return (best_found ? true : false);
}

void	Spider::move(const Vdb_test &world)
{
	static int phase = 0;
	static int pause = 0;
	if ((m_state & E_STATE_FALLING))
		return ;

	float		speed = 0.1;
	/*
	glm::vec3 dir;
	dir.z = 1.f;
	dir.x = 0.f;
	dir.y = 0.f;
	*/
	glm::vec3	dir = (m_target_world - m_pos);
	if (glm::length(dir) < 10.f)
		return ;
	dir = glm::normalize(dir);

	glm::vec3	vec = dir * 0.1f;
	vec = glm::vec3(0.f, 0.f, 1.f) * 0.1f;
	//m_pos += vec;
	glm::vec3	target;
	/*
	float		angle = glm::acos(glm::dot(glm::vec3(0.f, 0.f, 1.f), dir));
	m_bones[0] = glm::translate(glm::mat4(1.0f), m_bones_pos[0]);
	m_bones[0] = glm::rotate(m_bones[0], angle, glm::vec3(0.f, 1.f, 0.f));
	m_bones[0] = glm::translate(m_bones[0], -m_bones_pos[0]);
	*/
	if (pause >= 5)
	{
		for (int i = 0; i < 4; i++)
		{
			float lenx = (m_bones_pos[3 + i * 3].x
					- m_bones_pos[2 + i * 3].x) / 4.0f;
			target = m_bones_pos[2 + i * 3]
						+ glm::vec3(lenx, -10.0f, 0.0f);
			if ((i + phase) % 2)
				target += vec * 10.0f;
			else
				target -= vec * 10.0f;
			one_leg_move(i, target);
			lenx = (m_bones_pos[3 + (i + 4) * 3].x
					- m_bones_pos[2 + (i + 4) * 3].x) / 4.0f;
			target = m_bones_pos[2 + (i + 4) * 3]
						+ glm::vec3(lenx, -10.0f, 0.0f);
			if ((i + phase) % 2)
				target -= vec * 10.0f;
			else
				target += vec * 10.0f;
			one_leg_move(i + 4, target);
		}
		phase = (phase + 1) % 2;
		pause = -1;
	}
	pause++;
}

void	Spider::add_box(s_vbox box, uint32_t value)
{
	for (int x = 0; x < box.len.x; x++)
	{
		for (int y = 0; y < box.len.y; y++)
		{
			for (int z = 0; z < box.len.z; z++)
			{
				m_vdb.set_vox(value, s_vec3i(box.origin.x + x
					, box.origin.y + y, box.origin.z + z));
			}
		}
	}
}

void	Spider::generate()
{
	s_vbox	body;
	s_vbox	leg;
	int		bone;
	int		bone_pos;

	m_target_leg.resize(8);
	m_bones.resize(17, glm::mat4(1.0));
	m_bones_pos.resize(25, glm::vec3(0.0));
	body.origin = s_vec3i(50, 47, 1);
	body.len = s_vec3i(28, 16, 55);
	m_bones_pos[0] = glm::vec3(body.origin.x + body.len.x / 2.0f
							, body.origin.y + body.len.y / 2.0f
							, body.origin.z + body.len.z / 2.0f);
	m_center = m_bones_pos[0];
	add_box(body, 0);
	for (int i = 0; i < 4; i++)
	{
		leg.origin = s_vec3i(25, 50, 3 + i * ((body.len.z - 8) / 4.0));
		leg.len = s_vec3i(25, 3, 3);
		bone = i * 2 + 1;
		bone_pos = i * 3 + 1;
		add_box(leg, bone);
		m_bones_pos[bone_pos] = glm::vec3(leg.origin.x + leg.len.x
								, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
		leg.origin.x = 0;
		add_box(leg, bone + 1);
		m_bones_pos[bone_pos + 1] = glm::vec3(leg.origin.x + leg.len.x
								, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
		m_bones_pos[bone_pos + 2] = glm::vec3(leg.origin.x
								, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
		m_target_leg[i] = m_bones_pos[bone_pos + 2];

		leg.origin.x = 78;
		bone = 9 + i * 2;
		bone_pos = 13 + i * 3;
		add_box(leg, bone);
		m_bones_pos[bone_pos] = glm::vec3(leg.origin.x
								, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
		leg.origin.x = 78 + 25;
		add_box(leg, bone + 1);
		m_bones_pos[bone_pos + 1] = glm::vec3(leg.origin.x
								, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
		m_bones_pos[bone_pos + 2] = glm::vec3(leg.origin.x + leg.len.x
								, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
		m_target_leg[i + 4] = m_bones_pos[bone_pos + 2];
	}
	m_vdb.pruning();
	m_vdb.mesh(m_mesh);
	for (int i = 0; i < 8; i++)
	{
		float lenx = (m_bones_pos[3 + i * 3].x
						- m_bones_pos[2 + i * 3].x) / 4.0f;
		one_leg_move(i, m_bones_pos[2 + i * 3]
						+ glm::vec3(lenx, -10.0f, 0.0f));
	}
}

void	Spider::bones_test()
{
	int			bone_id;
	static int	t = 0;
	static int	t_add = 1;
	
	for (int i = 1; i < m_bones.size(); i++)
		m_bones[i] = glm::mat4(1.0f);
	if (t > 100)
		t_add = -1;
	if (t < -100)
		t_add = 1;
	t += t_add;
	for (int i = 0; i < 8; i++)
	{
		if (i > 3)
			one_leg_move(i, m_bones_pos[3 + i * 3]
					+ glm::vec3(10.0f, t / 10.f, 0.0f));
		else
			one_leg_move(i, m_bones_pos[3 + i * 3]
					- glm::vec3(10.0f, t / 10.f, 0.0f));
	}
}

void Spider::one_leg_move(int leg_id, glm::vec3 target)
{
	int	off = 1 + leg_id * 2;
	int	off_pos = 1 + leg_id * 3;
	std::vector<glm::mat4>::const_iterator first = m_bones.begin() + off;
	std::vector<glm::mat4>::const_iterator last = m_bones.begin() + off + 2;
	std::vector<glm::vec3>::const_iterator
					first_pos = m_bones_pos.begin() + off_pos;
	std::vector<glm::vec3>::const_iterator
					last_pos = m_bones_pos.begin() + off_pos + 3;
	std::vector<glm::mat4> leg_bones(first, last);
	std::vector<glm::vec3> leg_pos(first_pos, last_pos);
	Ccd_solver::ccd_solve(leg_bones, leg_pos, target);
	for (int i = 0; i < 2; i++)
	{
		m_bones[i + off] = m_bones[0] * leg_bones[i];
	}
	m_target_leg[leg_id] = glm::vec3(
							glm::vec4(leg_pos[2], 1.f) * m_bones[0] / 10.0f);
}
