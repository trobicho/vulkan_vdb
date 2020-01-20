/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Enemy_vbo.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/21 21:02:41 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/24 20:01:53 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "My_vulkan.h"
#include <iostream>
#include <cstring>

void	s_enemy::unload(My_vulkan &vulk)
{
	const auto	&device_ref = vulk.get_device_ref();

	vkDestroyBuffer(device_ref, m_vertex_index_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_index_buffer_memory, nullptr);
	vkDestroyBuffer(device_ref, m_vertex_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_buffer_memory, nullptr);
}

void	s_enemy::command_buffer_binder(VkCommandBuffer &cmd_buffer)
{
	VkBuffer vertex_buffers[] = {m_vertex_buffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(cmd_buffer, 0, 1
			, (const VkBuffer*)vertex_buffers, offsets);
	vkCmdBindIndexBuffer(cmd_buffer, m_vertex_index_buffer, 0
			, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmd_buffer, nb_index, 1, 0, 0, 0);
}

int		s_enemy::update(My_vulkan &vulk, Mesh<s_vertex_bones> &mesh)
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

int		s_enemy::alloc_buffer(My_vulkan &vulk, VkDeviceSize vbo_size
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
