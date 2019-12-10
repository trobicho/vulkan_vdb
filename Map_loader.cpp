/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:20 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/10 17:07:43 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Map_loader.h"
#include <iostream>
#include <cstring>

Map_loader::Map_loader(Vdb_test &my_vdb, My_vulkan &vulk, Player &player)
					: m_vdb(my_vdb), m_vulk(vulk), m_player(player)
					, m_map(0), m_moore_access(m_vdb)
{
	m_chunk.reserve(1024);
}

Map_loader::~Map_loader()
{
	for (int i = 0; i < m_nb_chunk; ++i)
		m_chunk[i].unload(m_vulk);
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

	while (!m_quit)
	{
		if (m_update == false)
		{
			search_new_chunk();
		}
	}
}

void	Map_loader::search_new_chunk()
{
	glm::vec3	p_pos = m_player.get_cam_pos();
	s_vec3i		i_pos = s_vec3i((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);
	s_vec3i		pos = i_pos;
	int			update = 0;

	update += load_pos(pos);
	while (next_chunk_in_radius(i_pos, pos, m_meshing_radius))
	{
		update += load_pos(pos);
	}
	std::cout << std::endl;
	if (update > 0)
		m_update = true;
}

void	Map_loader::unload_far_chunk()
{
	glm::vec3	p_pos = m_player.get_cam_pos();
	s_vec3i		i_pos = s_vec3i((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);
	int			i = 0;

	while (i < m_nb_chunk)
	{
		if (taxicab_chunk_dist(i_pos, m_chunk[i].origin)
			> m_unload_meshing_radius)
		{
			std::cout << taxicab_chunk_dist(i_pos, m_chunk[i].origin) << std::endl;
			if (m_chunk[i].in_vbo)
				m_chunk[i].unload(m_vulk);
			else
				m_chunk[i].mesh.reset();
			m_chunk.erase(m_chunk.begin() + i);
			m_nb_chunk--;
			continue;
		}
		i++;
	}
}

void	s_chunk::unload(My_vulkan &vulk)
{
	const auto	&device_ref = vulk.get_device_ref();

	vkDestroyBuffer(device_ref, m_vertex_index_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_index_buffer_memory, nullptr);
	vkDestroyBuffer(device_ref, m_vertex_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_buffer_memory, nullptr);
	mesh.reset();
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

	for (int i = 0; i < m_nb_chunk; ++i)
	{
		if (m_chunk[i].in_vbo && m_chunk[i].origin.x <= block.x
			&& m_chunk[i].origin.x + (1 << CHUNK_LOG_X) > block.x
			&& m_chunk[i].origin.y <= block.y
			&& m_chunk[i].origin.y + (1 << CHUNK_LOG_Y) > block.y
			&& m_chunk[i].origin.z <= block.z
			&& m_chunk[i].origin.z + (1 << CHUNK_LOG_Z) > block.z)
		{
			std::cout << "find the changing chunk ("
						<< m_chunk[i].origin.x << ", "
						<< m_chunk[i].origin.y << ", "
						<< m_chunk[i].origin.z << ")" << std::endl;
			m_chunk[i].reset(m_vulk);
			box.origin = m_chunk[i].origin;
			box.len = s_vec3i(1 << CHUNK_LOG_X, 1 << CHUNK_LOG_Y
					, 1 << CHUNK_LOG_Z);
			mesh_one_chunck(box, i);
			m_update = true;
			break;
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
		m_chunk.push_back(s_chunk(m_moore_access));
		mesh_one_chunck(box, m_nb_chunk);
		m_nb_chunk++;
		return (1);
	}
	else
	{
		box.len = s_vec3i(1 << CHUNK_LOG_X, 1 << CHUNK_LOG_Y
				, 1 << CHUNK_LOG_Z);
		box.origin = s_vec3i((pos.x >> CHUNK_LOG_X) << CHUNK_LOG_X
				, 0, (pos.z >> CHUNK_LOG_Z) << CHUNK_LOG_Z);
		for (int i = 0; i < m_nb_chunk; ++i)
		{
			if (m_chunk[i].origin.x == box.origin.x
				&& m_chunk[i].origin.z == box.origin.z)
				return (0);
		}
		m_chunk.push_back(s_chunk(m_moore_access));
		mesh_one_chunck(box, m_nb_chunk);
		m_nb_chunk++;
		return (1);
	}
	return (0);
}

int		Map_loader::update()
{
	unload_far_chunk();
	for (int i = 0; i < m_nb_chunk; ++i)
	{
		if (!m_chunk[i].in_vbo)
		{
			m_chunk[i].update(m_vulk);
		}
	}
	if (m_vulk.command_buffer_record(m_chunk) == -1)
		return (-1);
	m_update = false;
	return (0);
}

void	s_chunk::reset(My_vulkan &vulk)
{
	const auto	&device_ref = vulk.get_device_ref();

	vkDestroyBuffer(device_ref, m_vertex_index_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_index_buffer_memory, nullptr);
	vkDestroyBuffer(device_ref, m_vertex_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_buffer_memory, nullptr);
	mesh.reset();
}

void	s_chunk::command_buffer_binder(VkCommandBuffer &cmd_buffer)
{
	VkBuffer vertex_buffers[] = {m_vertex_buffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(cmd_buffer, 0, 1
			, (const VkBuffer*)vertex_buffers, offsets);
	vkCmdBindIndexBuffer(cmd_buffer, m_vertex_index_buffer, 0
			, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmd_buffer, mesh.index_buffer.size(), 1, 0, 0, 0);
}

int		s_chunk::update(My_vulkan &vulk)
{
	VkBuffer		staging_buffer;
	VkBuffer		staging_buffer_idx;
	VkDeviceMemory	staging_buffer_memory;
	VkDeviceMemory	staging_buffer_memory_idx;
	VkDeviceSize	copy_size;
	VkDeviceSize	copy_size_idx;
	void*			data;
	void*			data_idx;
	const VkDevice	&device_ref = vulk.get_device_ref();
	
	copy_size = mesh.vertex_buffer.size() * sizeof(mesh.vertex_buffer[0]);
	copy_size_idx = mesh.index_buffer.size() * sizeof(mesh.index_buffer[0]);
	if (alloc_buffer(vulk, copy_size, copy_size_idx) == -1)
		return (-1);
	if (vulk.create_buffer(copy_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, staging_buffer, staging_buffer_memory) == -1)
	{
		return (-1);
	}
	if (vulk.create_buffer(copy_size_idx, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, staging_buffer_idx, staging_buffer_memory_idx) == -1)
	{
		return (-1);
	}
	vkMapMemory(device_ref, staging_buffer_memory, 0, copy_size, 0, &data);
	vkMapMemory(device_ref, staging_buffer_memory_idx, 0, copy_size_idx, 0, &data_idx);
	memcpy(((char*)data), mesh.vertex_buffer.data(), copy_size);
	memcpy(((char*)data_idx), mesh.index_buffer.data(), copy_size_idx);
	vkUnmapMemory(device_ref, staging_buffer_memory);
	vkUnmapMemory(device_ref, staging_buffer_memory_idx);
	if (vulk.copy_staging_to_buffer(m_vertex_buffer
			, staging_buffer, staging_buffer_memory
			, copy_size) == -1)
		return (-1);
	if (vulk.copy_staging_to_buffer(m_vertex_index_buffer
			, staging_buffer_idx, staging_buffer_memory_idx
			, copy_size_idx) == -1)
		return (-1);
	in_vbo = true;
	return (0);
}

int		s_chunk::alloc_buffer(My_vulkan &vulk, VkDeviceSize vbo_size
			, VkDeviceSize ibo_size)
{
	if (vulk.create_buffer(vbo_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT
		| VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_vertex_buffer, m_vertex_buffer_memory) == -1)
	{
		return (-1);
	}
	if (vulk.create_buffer(ibo_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT
		| VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_vertex_index_buffer, m_vertex_index_buffer_memory) == -1)
	{
		return (-1);
	}
	return (0);
}

/*
int		Map_loader::update()
{
	VkBuffer		staging_buffer;
	VkBuffer		staging_buffer_idx;
	VkDeviceMemory	staging_buffer_memory;
	VkDeviceMemory	staging_buffer_memory_idx;
	VkDeviceSize	copy_size;
	VkDeviceSize	copy_size_idx;
	void*			data;
	void*			data_idx;
	const VkDevice	&device_ref = m_vulk.get_device_ref();

	copy_size = sizeof(m_mesh.vertex_buffer[0])
				* (m_mesh.vertex_buffer.size() - m_old_size_vbo);
	copy_size_idx = sizeof(m_mesh.index_buffer[0])
				* (m_mesh.index_buffer.size() - m_old_size_ibo);
	if (copy_size == 0 || copy_size_idx == 0)
	{
		m_update = false;
		return (0);
	}
	if (m_vulk.create_buffer(copy_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, staging_buffer, staging_buffer_memory) == -1)
	{
		return (-1);
	}
	if (m_vulk.create_buffer(copy_size_idx, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, staging_buffer_idx, staging_buffer_memory_idx) == -1)
	{
		return (-1);
	}
	vkMapMemory(device_ref, staging_buffer_memory, 0, copy_size, 0, &data);
	vkMapMemory(device_ref, staging_buffer_memory_idx, 0, copy_size_idx, 0, &data_idx);
	uint32_t	offset_data_vbo = 0;
	uint32_t	offset_data_ibo = 0;
	for (int i = 0; i < m_nb_chunk; ++i)
	{
		if (!m_chunk[i].in_vbo)
		{
			memcpy(((char*)data) + offset_data_vbo
					, m_mesh.vertex_buffer.data() + m_chunk[i].start_offset
					, m_chunk[i].size * sizeof(m_mesh.vertex_buffer[0]));
			memcpy(((char*)data_idx) + offset_data_ibo
					, m_mesh.index_buffer.data() + m_chunk[i].start_offset_idx
					, m_chunk[i].size_idx * sizeof(m_mesh.index_buffer[0]));
			offset_data_vbo += m_chunk[i].size * sizeof(m_mesh.vertex_buffer[0]);
			offset_data_ibo += m_chunk[i].size_idx * sizeof(m_mesh.index_buffer[0]);
			m_chunk[i].in_vbo = true;
		}
	}
	vkUnmapMemory(device_ref, staging_buffer_memory);
	vkUnmapMemory(device_ref, staging_buffer_memory_idx);
	m_vulk.copy_staging_to_vbo(staging_buffer, staging_buffer_memory
			, copy_size, m_old_size_vbo * sizeof(m_mesh.vertex_buffer[0]));
	m_vulk.copy_staging_to_ibo(staging_buffer_idx, staging_buffer_memory_idx
			, copy_size_idx, m_old_size_ibo * sizeof(m_mesh.index_buffer[0]));
	if (m_vulk.command_buffer_record(m_mesh.index_buffer.size()) == -1)
		return (-1);
	m_old_size_vbo = m_mesh.vertex_buffer.size();
	m_old_size_ibo = m_mesh.index_buffer.size();
	if (m_vulk.command_buffer_record(m_mesh.index_buffer.size()) == -1)
		return (-1);
	m_update = false;
	return (0);
}
*/

int		Map_loader::generate_one_chunck(s_vbox &box)
{
	auto	time = std::chrono::high_resolution_clock::now();
	if (m_map.generate(m_vdb, box))
		return (1);
	auto	gtime = std::chrono::high_resolution_clock::now();
	auto	time_gen = std::chrono::duration<float
			, std::chrono::seconds::period>(gtime - time).count();
	time = std::chrono::high_resolution_clock::now();
	m_vdb.pruning(); // one?
	std::cout << "time to generate = " << time_gen << std::endl;
	return (0);
}

int		Map_loader::mesh_one_chunck(s_vbox &box)
{
	if (m_nb_chunk < MAX_CHUNK)
	{
		m_chunk.push_back(s_chunk(m_moore_access));
		mesh_one_chunck(box, m_nb_chunk);
		m_update = true;
		m_nb_chunk++;
		return (1);
	}
	return (0);
}

int		Map_loader::mesh_one_chunck(s_vbox &box, uint32_t chunk_id)
{
	auto	time = std::chrono::high_resolution_clock::now();
	m_vdb.mesh(m_chunk[chunk_id].mesh, box);
	auto	mtime = std::chrono::high_resolution_clock::now();
	auto	time_mesh = std::chrono::duration<float
		, std::chrono::seconds::period>(mtime - time).count();
	std::cout << "time to mesh = " << time_mesh << std::endl;
	m_chunk[chunk_id].origin = box.origin;
	m_chunk[chunk_id].in_vbo = false;
	return (0);
}
