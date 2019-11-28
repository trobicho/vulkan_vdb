/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:20 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/28 20:16:47 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Map_loader.h"
#include <iostream>

Map_loader::Map_loader(Vdb_test &my_vdb, Player &player): m_vdb(my_vdb)
					, m_player(player), m_map(0), m_moore_access(m_vdb)
					, m_mesh(m_moore_access)
{
	m_mesh.reset();
}

static s_vec3i
		dist_from_chunk(s_vec3i pos, s_chunk &chunk)
{
	s_vec3i		d;

	d.x = chunk.origin.x - pos.x;
	if (d.x < 0)
		d.x = -d.x - (1 << CHUNK_LOG_X);
	d.y = chunk.origin.y - pos.y;
	if (d.y < 0)
		d.y = -d.y - (1 << CHUNK_LOG_Y);
	d.z = chunk.origin.z - pos.z;
	if (d.z < 0)
		d.z = -d.z - (1 << CHUNK_LOG_Z);
	return (d);
}

void	Map_loader::thread_loader()
{
	while (!m_quit)
	{
		if (m_update == false)
		{
			glm::vec3	p_pos = m_player.get_cam_pos();
			s_vec3i	i_pos = s_vec3i((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);
			s_vec3i	dist = dist_from_chunk(i_pos, m_chunk);
			std::cout << "dist = " << dist.x << ", " << dist.z << std::endl;
			std::cout << "p_pos = " << p_pos.x << ", " << p_pos.z << std::endl;
			if (dist.x > (1 << CHUNK_LOG_X) || dist.z > (1 << CHUNK_LOG_Z))
			{
				std::cout << "load new chunk" << std::endl;
				load_one_chunck(i_pos.x, i_pos.z);
				m_update = true;
			}
		}
	}
}

void	Map_loader::update()
{
	m_update = false;
}

int		Map_loader::load_one_chunck(uint32_t x, uint32_t z)
{
	s_vbox		box;

	auto	time = std::chrono::high_resolution_clock::now();
	box.len = s_vec3i(1 << CHUNK_LOG_X, 1 << CHUNK_LOG_Y, 1 << CHUNK_LOG_Z);
	box.origin = s_vec3i((x >> CHUNK_LOG_X) << CHUNK_LOG_X
					, 0, (z >> CHUNK_LOG_Z) << CHUNK_LOG_Z);
	if (m_map.generate(m_vdb, box))
		return (1);
	auto	gtime = std::chrono::high_resolution_clock::now();
	auto	time_gen = std::chrono::duration<float
			, std::chrono::seconds::period>(gtime - time).count();
	m_vdb.pruning();
	std::cout << "time to generate = " << time_gen << std::endl;
	auto	mtime = std::chrono::high_resolution_clock::now();
	auto	time_mesh = std::chrono::duration<float
			, std::chrono::seconds::period>(mtime - time).count();
	std::cout << "time to mesh = " << time_mesh << std::endl;
	std::cout << "total = " << time_mesh + time_gen << std::endl;
	m_vdb.mesh(m_mesh, box);
	m_chunk.origin = box.origin;
	return (0);
}
