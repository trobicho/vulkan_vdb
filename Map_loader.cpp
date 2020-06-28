/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:20 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/28 14:31:00 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Map_loader.h"
#include <iostream>
#include <cstring>
#include <thread>
#include <unistd.h>

Map_loader::Map_loader(Vdb_test &my_vdb, My_vulkan &vulk, Player &player)
					: m_vdb(my_vdb), m_vulk(vulk), m_player(player)
					, m_map(0), m_moore_access(m_vdb)
{
	m_chunk.reserve(1024);
}

Map_loader::~Map_loader()
{
	t_chunk_cont:: iterator	chunk;
	for (chunk = m_chunk.begin(); chunk != m_chunk.end(); ++chunk)
		chunk->second.unload(m_vulk);
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
	int up;
	int i = 0;

	std::thread thread(&Map_loader::unload_far_chunk, this);
	while (!m_quit)
	{
		if (m_update == false)
		{
			int up;
			m_update_mutex.lock();
			up = search_new_chunk();
			tag_unload_far_chunk();
			if (up > 5)
				m_update = true;
			m_update_mutex.unlock();
			i++;
		}
	}
	thread.join();
}

int		Map_loader::search_new_chunk()
{
	glm::vec3	p_pos = m_player.get_cam_pos();
	s_vec3i		i_pos = s_vec3i((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);
	s_vec3i		pos = i_pos;
	int			update = 0;

	auto chunk = m_chunk.find(
			std::make_pair((pos.x >> CHUNK_LOG_X) << CHUNK_LOG_X
					, (pos.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z));
	if (chunk == m_chunk.end())
		update += load_pos(pos);
	while (next_chunk_in_radius(i_pos, pos, m_meshing_radius) && !m_update)
	{
		auto chunk = m_chunk.find(
				std::make_pair((pos.x >> CHUNK_LOG_X) << CHUNK_LOG_X
					, (pos.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z));
		if (chunk == m_chunk.end())
			update += load_pos(pos);
		if (update > 15)
			break;
	}
	return (update);
}

void	Map_loader::unload_far_chunk()
{
	int i = 0;
	t_chunk_cont:: iterator	chunk_it;

	while (!m_quit)
	{
		if (m_need_unload > 10)
		{
			for (chunk_it = m_chunk.begin(); chunk_it != m_chunk.end()
					&& m_need_unload > 0;)
			{
				if (chunk_it->second.has_unload)
				{
					if (chunk_it->second.in_vbo)
						chunk_it->second.unload(m_vulk);
					else
						chunk_it->second.mesh.reset();
					chunk_it = m_chunk.erase(chunk_it);
					m_nb_chunk--;
					m_need_unload--;
				}
				else
					++chunk_it;
			}
		}
		if (m_need_unload < 0)
			m_need_unload = 0;
		usleep(100000);
	}
}

void	Map_loader::tag_unload_far_chunk()
{
	glm::vec3	p_pos = m_player.get_cam_pos();
	s_vec3i		i_pos = s_vec3i((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);
	int			i = 0;

	t_chunk_cont:: iterator	chunk_it;
	for (chunk_it = m_chunk.begin(); chunk_it != m_chunk.end(); ++chunk_it)
	{
		if (taxicab_chunk_dist(i_pos, chunk_it->second.origin)
			> m_unload_meshing_radius)
			chunk_it->second.need_unload = true;
		if (m_update)
			break;
	}
}

int		Map_loader::next_chunk_in_radius(s_vec3i center
			, s_vec3i &pos, int radius)
{
	int dx = (pos.x >> CHUNK_LOG_X) - (center.x >> CHUNK_LOG_X);
	int dz = (pos.z >> CHUNK_LOG_Z) - (center.z >> CHUNK_LOG_Z);

	if (dz == 0 && dx >= 0)
	{
		pos.z -= (1 << CHUNK_LOG_Z);
		if (taxicab_chunk_dist(center, pos) > radius)
			return (0);
	}
	else
	{
		if (dx > 0)
			pos.z -= (1 << CHUNK_LOG_Z);
		else if (dx == 0)
			pos.z += (dz > 0 ? -(1 << CHUNK_LOG_Z) : (1 << CHUNK_LOG_Z));
		else
			pos.z += (1 << CHUNK_LOG_Z);
		if (dz > 0)
			pos.x += (1 << CHUNK_LOG_X);
		else if (dz == 0)
			pos.x += (dx > 0 ? -(1 << CHUNK_LOG_X) : (1 << CHUNK_LOG_X));
		else
			pos.x -= (1 << CHUNK_LOG_X);
	}
	return (1);
}

void	Map_loader::block_change(s_vec3i block)
{
	s_vbox			box;

	auto chunk = m_chunk.find(
			std::make_pair((block.x >> CHUNK_LOG_X) << CHUNK_LOG_X
					, (block.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z));
	if (chunk != m_chunk.end())
	{
		std::cout << "find the changing chunk ("
			<< chunk->second.origin.x << ", "
			<< chunk->second.origin.y << ", "
			<< chunk->second.origin.z << ")" << std::endl;
		chunk->second.reset(m_vulk);
		box.origin = chunk->second.origin;
		box.len = s_vec3i(1 << CHUNK_LOG_X, 1 << CHUNK_LOG_Y
				, 1 << CHUNK_LOG_Z);
		std::lock_guard<std::mutex> guard(m_mesh_mutex);
		if (mesh_one_chunck(box) != -1)
			m_update = true;
		/*-------------
		  remesh adj_chunk as to check if solid
		*/
		if ((block.x >> CHUNK_LOG_X) << CHUNK_LOG_X == block.x)
		{
			std::cout << "mesh adj x - 1" << std::endl;
			chunk = m_chunk.find(
					std::make_pair(((block.x >> CHUNK_LOG_X) - 1) << CHUNK_LOG_X
						, (block.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z));
			chunk->second.reset(m_vulk);
			box.origin = chunk->second.origin;
			if (mesh_one_chunck(box) != -1)
				m_update = true;
		}
		else if (((block.x >> CHUNK_LOG_X) + 1) << CHUNK_LOG_X
				== block.x + 1)
		{
			std::cout << "mesh adj x + 1" << std::endl;
			chunk = m_chunk.find(
					std::make_pair(((block.x >> CHUNK_LOG_X) + 1) << CHUNK_LOG_X
						, (block.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z));
			chunk->second.reset(m_vulk);
			box.origin = chunk->second.origin;
			if (mesh_one_chunck(box) != -1)
				m_update = true;
		}
		if ((block.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z == block.z)
		{
			std::cout << "mesh adj z - 1" << std::endl;
			chunk = m_chunk.find(
					std::make_pair((block.x >> CHUNK_LOG_X) << CHUNK_LOG_X
						, ((block.z >> CHUNK_LOG_Z) - 1) << CHUNK_LOG_Z));
			chunk->second.reset(m_vulk);
			box.origin = chunk->second.origin;
			if (mesh_one_chunck(box) != -1)
				m_update = true;
		}
		else if (((block.z >> CHUNK_LOG_Z) + 1) << CHUNK_LOG_Z
				== block.z + 1)
		{
			std::cout << "mesh adj z + 1" << std::endl;
			chunk = m_chunk.find(
					std::make_pair((block.x >> CHUNK_LOG_X) << CHUNK_LOG_X
						, ((block.z >> CHUNK_LOG_Z) + 1) << CHUNK_LOG_Z));
			chunk->second.reset(m_vulk);
			box.origin = chunk->second.origin;
			if (mesh_one_chunck(box) != -1)
				m_update = true;
		}
	}
}

int		Map_loader::load_pos(s_vec3i pos)
{
	uint32_t		value;
	s_vbox			box;
	const Node_v	*node;

	if ((node = m_vdb.get_interresting_node(s_vec3i(pos.x, 0, pos.z), value))
			!= nullptr && node->get_child_slog().x >= CHUNK_LOG_X)
	{
		box.len = s_vec3i(1 << CHUNK_LOG_X, 1 << CHUNK_LOG_Y
				, 1 << CHUNK_LOG_Z);
		box.origin = s_vec3i((pos.x >> CHUNK_LOG_X) << CHUNK_LOG_X
				, 0, (pos.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z);
		generate_one_chunck(box);
		s_vbox	box2 = box;
		box2.origin.x += (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		box2.origin.z += (1 << CHUNK_LOG_Z);
		generate_one_chunck(box2);
		box2.origin.x -= (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		box2.origin.x -= (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		box2.origin.z -= (1 << CHUNK_LOG_Z);
		generate_one_chunck(box2);
		box2.origin.z -= (1 << CHUNK_LOG_Z);
		generate_one_chunck(box2);
		box2.origin.x += (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		box2.origin.x += (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		std::lock_guard<std::mutex> guard(m_mesh_mutex);
		if (m_update)
			return (0);
		if (mesh_one_chunck(box, m_chunk.insert(
				{std::make_pair(box.origin.x, box.origin.z)
				, s_chunk(m_moore_access)}).first->second) == -1)
				return (0);
		m_nb_chunk++;
		return (1);
	}
	else
	{
		box.len = s_vec3i(1 << CHUNK_LOG_X, 1 << CHUNK_LOG_Y
				, 1 << CHUNK_LOG_Z);
		box.origin = s_vec3i((pos.x >> CHUNK_LOG_X) << CHUNK_LOG_X
				, 0, (pos.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z);
		auto chunk = m_chunk.find(std::make_pair(box.origin.x, box.origin.z));
		if (chunk != m_chunk.end())
		{
			if (chunk->second.need_unload)
				chunk->second.need_unload = false;
				return (0);
		}
		std::lock_guard<std::mutex> guard(m_mesh_mutex);
		if (m_update)
			return (0);
		s_vbox	box2 = box;
		box2.origin.x += (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		box2.origin.z += (1 << CHUNK_LOG_Z);
		generate_one_chunck(box2);
		box2.origin.x -= (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		box2.origin.x -= (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		box2.origin.z -= (1 << CHUNK_LOG_Z);
		generate_one_chunck(box2);
		box2.origin.z -= (1 << CHUNK_LOG_Z);
		generate_one_chunck(box2);
		box2.origin.x += (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		box2.origin.x += (1 << CHUNK_LOG_X);
		generate_one_chunck(box2);
		if (mesh_one_chunck(box, m_chunk.insert(
				{std::make_pair(box.origin.x, box.origin.z)
				, s_chunk(m_moore_access)}).first->second) == -1)
				return (0);
		m_nb_chunk++;
		return (1);
	}
	return (0);
}

int		Map_loader::update()
{
	t_chunk_cont:: iterator	chunk;
	std::lock_guard<std::mutex> guard(m_update_mutex);

	for (chunk = m_chunk.begin(); chunk != m_chunk.end(); ++chunk)
	{
		if (!chunk->second.in_vbo && !chunk->second.need_unload)
		{
			chunk->second.update(m_vulk);
		}
		else if (chunk->second.need_unload && !chunk->second.has_unload)
		{
			chunk->second.has_unload = true;
			m_need_unload++;
		}
	}
	if (m_vulk.command_buffer_record(m_chunk) == -1)
		return (-1);
	std::cout << "update: " << m_nb_chunk << std::endl;
	m_update = false;
	return (0);
}

int		Map_loader::generate_one_chunck(s_vbox &box)
{
	const Node_v	*node;
	uint32_t		value;

	if ((node = m_vdb.get_interresting_node(box.origin, value))
			!= nullptr && node->get_child_slog().x >= CHUNK_LOG_X)
	{
		if (m_map.generate(m_vdb, box))
			return (1);
		/*
		auto	time = std::chrono::high_resolution_clock::now();
		if (m_map.generate(m_vdb, box))
			return (1);
		auto	gtime = std::chrono::high_resolution_clock::now();
		auto	time_gen = std::chrono::duration<float
			, std::chrono::seconds::period>(gtime - time).count();
		time = std::chrono::high_resolution_clock::now();
		*/
		m_vdb.pruning(); // one?
		//std::cout << "time to generate = " << time_gen << std::endl;
	}
	return (0);
}

int		Map_loader::mesh_one_chunck(s_vbox &box)
{
	if (m_chunk.empty())
	{
		if (m_nb_chunk < MAX_CHUNK)
		{
			if (mesh_one_chunck(box, m_chunk.insert(
						{std::make_pair(box.origin.x, box.origin.z)
						, s_chunk(m_moore_access)}).first->second) == -1)
			{
				return (-1);
			}
		}
		m_update = true;
		m_nb_chunk++;
	}
	else
	{
		auto chunk = m_chunk.find(std::make_pair(box.origin.x, box.origin.z));
		if (chunk != m_chunk.end())
		{
			if (m_nb_chunk < MAX_CHUNK)
			{
				if (mesh_one_chunck(box, m_chunk.insert(
							{std::make_pair(box.origin.x, box.origin.z)
							, s_chunk(m_moore_access)}).first->second) == -1)
				{
					return (-1);
				}
			}
			m_update = true;
			m_nb_chunk++;
		}
	}
	return (0);
}

int		Map_loader::mesh_one_chunck(s_vbox &box, s_chunk& chunk)
{
	m_vdb.mesh(chunk.mesh, box);
	chunk.is_reset = false;
	/*
	auto	time = std::chrono::high_resolution_clock::now();
	m_vdb.mesh(chunk.mesh, box);
	auto	mtime = std::chrono::high_resolution_clock::now();
	auto	time_mesh = std::chrono::duration<float
		, std::chrono::seconds::period>(mtime - time).count();
	std::cout << "time to mesh = " << time_mesh << std::endl;
	*/
	if (chunk.mesh.vertex_buffer.size() == 0
		|| chunk.mesh.index_buffer.size() == 0)
	{
		m_chunk.erase({std::make_pair(box.origin.x, box.origin.z)});
		return (-1);
	}
	chunk.origin = box.origin;
	chunk.in_vbo = false;
	return (0);
}
