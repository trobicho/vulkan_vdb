/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 22:08:47 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/08 19:42:25 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Spider.h"
#include "Ccd_solver.h"

Spider::Spider(glm::vec3 pos): m_mesh(m_moore_access), Character(8), m_pos(pos)
{
}

bool	Spider::check_ground(const Vdb_test &world)
{
	return (true);
	s_vec3i		vox((int)m_pos.x, (int)m_pos.y - 1, (int)m_pos.z);
	glm::vec3	voxi;
	int			found = 0;
	int			best_found = 0;

	for (int i = 0; i < m_feet_target_world.size(); i++)
	{
		voxi = m_feet_target_world[i];
		vox.x = (int)voxi.x;
		vox.y = (int)voxi.y;
		vox.z = (int)voxi.z;
		if ((found = world.get_vox(vox)))
		{
			m_pos.y += ((float)vox.y + 1.0f) - m_feet_target_world[i].y;
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
		m_state &= (~CHAR_STATE_FALLING);
	else
		m_state |= CHAR_STATE_FALLING;
	return (best_found ? true : false);
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

	m_feet_target_world.resize(8);
	m_bones.resize(17, glm::mat4(1.0));
	m_bones_pos.resize(25, glm::vec3(0.0));
	body.origin = s_vec3i(50, 47, 17);
	body.len = s_vec3i(28, 16, 30);
	m_bones_pos[0] = glm::vec3(body.origin.x + body.len.x / 2.0f
							, body.origin.y + body.len.y / 2.0f
							, body.origin.z + body.len.z / 2.0f);
	m_center = m_bones_pos[0];
	m_feet_gate.len = 20;
	add_box(body, 0);
	for (int i = 0; i < 4; i++)
	{
		leg.len = s_vec3i(25, 3, 3);
		leg.origin = s_vec3i(25, body.origin.y + body.len.y / 2.0 - leg.len.y
						, body.origin.z + 4 + i * ((body.len.z - 8) / 4.0));
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
		m_feet_target_world[i] = m_bones_pos[bone_pos + 2];
		m_feet_info[i].right_side = false;
		m_feet_info[i].base_angle_torso = 0.51f + -0.4f * i;
		m_feet_info[i].base_height = 30.f;
		m_feet_info[i].base_dist_torso = leg.len.x + body.len.x / 2.0f;
		m_feet_info[i].begin_gate = (i % 2) ? 0 : 10;
		m_feet_info[i].end_gate = (i % 2) ? 10 : 20;

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
		m_feet_target_world[i + 4] = m_bones_pos[bone_pos + 2];
		m_feet_info[i + 4].right_side = true;
		m_feet_info[i + 4].base_angle_torso = -0.51f + 0.4f * i;
		m_feet_info[i + 4].base_height = 30.f;
		m_feet_info[i + 4].base_dist_torso = leg.len.x + body.len.x / 2.0f;
		m_feet_info[i + 4].begin_gate = (!(i % 2)) ? 0 : 10;
		m_feet_info[i + 4].end_gate = (!(i % 2)) ? 10 : 20;
	}
	m_vdb.pruning();
	m_vdb.mesh(m_mesh);
	for (int i = 0; i < 8; i++)
	{
		float lenx = (m_bones_pos[3 + i * 3].x
						- m_bones_pos[2 + i * 3].x) / 4.0f;
		foot_to_target_relative(i, m_bones_pos[2 + i * 3]
						+ glm::vec3(lenx, -20.0f, 0.0f));
	}
}

void	Spider::ik_all()
{
	for (int i = 0; i < 8; i++)
	{
		foot_to_target_world(i, m_feet_target_world[i]);
	}
}

void	Spider::foot_to_target_world(int foot_id, glm::vec3 target_world)
{
	glm::vec3	target = glm::vec3(glm::vec4((target_world - m_pos) * 10.f, 1.f)
							/ m_bones[0]);

	foot_to_target_relative(foot_id, target);
}

void	Spider::foot_to_target_relative(int leg_id, glm::vec3 target)
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
	m_feet_target_world[leg_id] = glm::vec3(
							glm::vec4(leg_pos[2], 1.f) * m_bones[0] / 10.0f)
							+ m_pos;
}
