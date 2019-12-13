/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chunk.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/11 14:27:32 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/13 17:56:52 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "My_vulkan.h"
#include <iostream>
#include <cstring>

void	s_chunk::unload(My_vulkan &vulk)
{
	const auto	&device_ref = vulk.get_device_ref();

	vkDestroyBuffer(device_ref, m_vertex_index_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_index_buffer_memory, nullptr);
	vkDestroyBuffer(device_ref, m_vertex_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_buffer_memory, nullptr);
	mesh.reset();
}

void	s_chunk::reset(My_vulkan &vulk)
{
	const auto	&device_ref = vulk.get_device_ref();

	vkDestroyBuffer(device_ref, m_vertex_index_buffer, nullptr);
	vkFreeMemory(device_ref, m_vertex_index_buffer_memory, nullptr);
	if (has_blend)
	{
		vkDestroyBuffer(device_ref, m_index_buffer_blend, nullptr);
		vkFreeMemory(device_ref, m_index_buffer_blend_memory, nullptr);
	}
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

void	s_chunk::command_buffer_binder_blend(VkCommandBuffer &cmd_buffer)
{
	VkBuffer vertex_buffers[] = {m_vertex_buffer};
	VkDeviceSize offsets[] = {0};
	if (has_blend)
	{
		vkCmdBindVertexBuffers(cmd_buffer, 0, 1
				, (const VkBuffer*)vertex_buffers, offsets);
		vkCmdBindIndexBuffer(cmd_buffer, m_index_buffer_blend, 0
				, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(cmd_buffer, mesh.index_buffer_blend.size(), 1, 0, 0, 0);
	}
}

int		s_chunk::update(My_vulkan &vulk)
{
	VkBuffer		staging_buffer;
	VkBuffer		staging_buffer_idx;
	VkDeviceMemory	staging_buffer_memory;
	VkDeviceMemory	staging_buffer_memory_idx;
	VkDeviceSize	copy_size;
	VkDeviceSize	copy_size_idx;
	VkDeviceSize	copy_size_blend = 0;
	void*			data;
	void*			data_idx;
	const VkDevice	&device_ref = vulk.get_device_ref();

	if (mesh.index_buffer_blend.size() > 0)
		has_blend = true;
	if (has_blend)
	{
		copy_size_blend = mesh.index_buffer_blend.size()
			* sizeof(mesh.index_buffer_blend[0]);
	}
	copy_size = mesh.vertex_buffer.size() * sizeof(mesh.vertex_buffer[0]);
	copy_size_idx = mesh.index_buffer.size() * sizeof(mesh.index_buffer[0]);
	if (alloc_buffer(vulk, copy_size, copy_size_idx, copy_size_blend) == -1)
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
	if (has_blend)
	{
		VkBuffer		staging_buffer_blend;
		VkDeviceMemory	staging_buffer_memory_blend;
		void*			data_blend;
		if (vulk.create_buffer(copy_size_blend, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, staging_buffer_blend, staging_buffer_memory_blend) == -1)
		{
			return (-1);
		}
		vkMapMemory(device_ref, staging_buffer_memory_blend
					, 0, copy_size_blend, 0, &data_blend);
		memcpy(((char*)data_blend), mesh.index_buffer_blend.data()
				, copy_size_blend);
		vkUnmapMemory(device_ref, staging_buffer_memory_blend);
		if (vulk.copy_staging_to_buffer(m_index_buffer_blend
				, staging_buffer_blend, staging_buffer_memory_blend
				, copy_size_blend) == -1)
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
			, VkDeviceSize ibo_size, VkDeviceSize blend_size)
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
	if (blend_size > 0)
	{
		if (vulk.create_buffer(blend_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			| VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_index_buffer_blend, m_index_buffer_blend_memory) == -1)
		{
			return (-1);
		}
	}
	return (0);
}
