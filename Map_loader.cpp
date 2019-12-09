/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:20 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/09 08:29:54 by trobicho         ###   ########.fr       */
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
			up = 0;
			glm::vec3	p_pos = m_player.get_cam_pos();
			s_vec3i	i_pos = s_vec3i((int)p_pos.x, (int)p_pos.y, (int)p_pos.z);
			up = load_pos(i_pos);
			i_pos = s_vec3i((int)p_pos.x + (1 << CHUNK_LOG_X), (int)p_pos.y
					, (int)p_pos.z);
			up += load_pos(i_pos);
			i_pos = s_vec3i((int)p_pos.x - (1 << CHUNK_LOG_X), (int)p_pos.y
					, (int)p_pos.z);
			up += load_pos(i_pos);
			i_pos = s_vec3i((int)p_pos.x, (int)p_pos.y
					, (int)p_pos.z + (1 << CHUNK_LOG_Z));
			up += load_pos(i_pos);
			i_pos = s_vec3i((int)p_pos.x, (int)p_pos.y
					, (int)p_pos.z - (1 << CHUNK_LOG_Z));
			up += load_pos(i_pos);
			if (up > 0)
			{
				/*
				std::cout << "new vbo size = " << m_mesh.vertex_buffer.size()
					<< std::endl;
				std::cout << "new ibo size = " << m_mesh.index_buffer.size()
					<< std::endl << std::endl;
					*/
				m_update = true;
			}
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
		std::cout << "load new chunk" << std::endl;
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
	return (0);
}

int		Map_loader::update()
{
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
