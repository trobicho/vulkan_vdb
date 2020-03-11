/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 22:08:47 by trobicho          #+#    #+#             */
/*   Updated: 2020/03/11 11:58:44 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Spider.h"

Spider::Spider(): m_mesh(m_moore_access)
{
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

	m_bones.resize(17, glm::mat4(1.0));
	m_bones_pos.resize(17, glm::vec3(0.0));
	body.origin = s_vec3i(50, 47, 1);
	body.len = s_vec3i(28, 16, 55);
	m_bones_pos[0] = glm::vec3(body.origin.x + body.len.x / 2.0f
							, body.origin.y + body.len.y / 2.0f
							, body.origin.z + body.len.z / 2.0f);
	add_box(body, 0);
	for (int i = 0; i < 4; i++)
	{
		leg.origin = s_vec3i(25, 50, 3 + i * ((body.len.z - 8) / 4.0));
		leg.len = s_vec3i(25, 3, 3);
		bone = i * 2 + 1;
		add_box(leg, bone);
		m_bones_pos[bone] = glm::vec3(50, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
		leg.origin = s_vec3i(0, 50, 3 + i * ((body.len.z - 8) / 4.0));
		add_box(leg, bone + 1);
		m_bones_pos[bone + 1] = glm::vec3(25, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);

		leg.origin.x = 78;
		bone = 9 + i * 2;
		add_box(leg, bone);
		m_bones_pos[bone] = glm::vec3(leg.origin.x, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
		leg.origin.x = 78 + 25;
		add_box(leg, bone + 1);
		m_bones_pos[bone + 1] = glm::vec3(leg.origin.x, leg.origin.y + leg.len.y / 2.0f
								, leg.origin.z + leg.len.z / 2.0f);
	}
	m_vdb.pruning();
	m_vdb.mesh(m_mesh);
}

void	Spider::bones_test()
{
	int			bone_id;
	static int	t = 0;
	static int	t_add = 1;

	m_bones[0] = glm::translate(glm::mat4(1.0f), m_bones_pos[0]);
	m_bones[0] = glm::rotate(m_bones[0]
			, 0.01f * t , glm::vec3(0.0f, 1.0f, 0.0f));
	m_bones[0] = glm::translate(m_bones[0]
					, glm::vec3(-m_bones_pos[0].x
						, -m_bones_pos[0].y, -m_bones_pos[0].z));
	for (bone_id = 1; bone_id < 17; bone_id++)
	{
		float	angle = 0.01f;
		if (bone_id > 8)
			angle = 0 - angle;
		if (bone_id % 2)
			m_bones[bone_id] = glm::translate(m_bones[0], m_bones_pos[bone_id]);
		else
			m_bones[bone_id] = glm::translate(m_bones[bone_id - 1], m_bones_pos[bone_id]);
		m_bones[bone_id] = glm::rotate(m_bones[bone_id]
				, angle * (t + (bone_id % 5) * 3), glm::vec3(0.0f, 0.0f, 1.0f));
		m_bones[bone_id] = glm::translate(m_bones[bone_id]
				, glm::vec3(-m_bones_pos[bone_id].x
					, -m_bones_pos[bone_id].y, -m_bones_pos[bone_id].z));
	}
	if (t > 100)
		t_add = -1;
	if (t < -100)
		t_add = 1;
	t += t_add;
}
