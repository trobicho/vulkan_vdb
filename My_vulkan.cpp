/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   My_vulkan.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/09 17:05:36 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/17 05:27:42 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "My_vulkan.h"
#include "init_swap_chain.h"
#include "shader.h"
#include "gpu_pipeline.h"
#include "validation_layer.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

My_vulkan::My_vulkan(GLFWwindow *win, Mesh &mesh, s_ubo &ubo):
	m_win(win), m_mesh(mesh), m_ubo(ubo) //try catch
{
}

My_vulkan::~My_vulkan()
{
	int			i;
	uint32_t	img_count;

	vkDestroySemaphore(m_device, m_semaphore_render_finish, nullptr);
	vkDestroyCommandPool(m_device, m_command_pool, nullptr);
	vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
	vkDestroyRenderPass(m_device, m_render_pass, nullptr);
	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &img_count, nullptr);
	i = 0;
	while (i < img_count)
	{
		vkDestroyImageView(m_device, m_image_view[i], nullptr);
		vkDestroyFramebuffer(m_device, m_framebuffer[i], nullptr);
		vkDestroyBuffer(m_device, m_uniform_buffer[i], nullptr);
		vkFreeMemory(m_device, m_uniform_buffer_memory[i], nullptr);
		i++;
	}
	vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);
	vkDestroySampler(m_device, m_texture_sampler, nullptr);
	vkDestroyImageView(m_device, m_texture_img_view, nullptr);
	vkDestroyImage(m_device, m_texture_img, nullptr);
	vkDestroyImageView(m_device, m_depth_img_view, nullptr);
	vkDestroyImage(m_device, m_depth_img, nullptr);
	vkFreeMemory(m_device, m_depth_img_memory, nullptr);
	vkFreeMemory(m_device, m_texture_img_memory, nullptr);
	vkDestroyDescriptorSetLayout(m_device, m_desc_set_layout, nullptr);
	vkDestroyBuffer(m_device, m_vertex_index_buffer, nullptr);
	vkFreeMemory(m_device, m_vertex_index_buffer_memory, nullptr);
	vkDestroyBuffer(m_device, m_vertex_buffer, nullptr);
	vkFreeMemory(m_device, m_vertex_buffer_memory, nullptr);
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);
	vkDestroyDevice(m_device, nullptr);
	glfwDestroyWindow(m_win);
	glfwTerminate();
}

int My_vulkan::draw_frame()
{
	uint32_t				img_index;
	VkSubmitInfo			submit_info;
	VkPipelineStageFlags	wait_stage;
	VkPresentInfoKHR		present_info;

	vkAcquireNextImageKHR(m_device, m_swap_chain, UINT64_MAX,
		m_semaphore_image_available, VK_NULL_HANDLE, &img_index);
	update_uniform_buffer(img_index);
	submit_info = (VkSubmitInfo){};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &m_semaphore_image_available;
	wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit_info.pWaitDstStageMask = &wait_stage;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_command_buffer[img_index];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &m_semaphore_render_finish;
	if (vkQueueSubmit(m_queue_graphic, 1, &submit_info, VK_NULL_HANDLE)
		!= VK_SUCCESS)
	{
		printf("failed to submit draw command buffer!\n");
		return (-1);
	}
	present_info = (VkPresentInfoKHR){};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &m_semaphore_render_finish;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &m_swap_chain;
	present_info.pImageIndices = &img_index;
	present_info.pResults = NULL;
	vkQueuePresentKHR(m_queue_graphic, &present_info);
	vkQueueWaitIdle(m_queue_graphic);
	return (0);
}

int		My_vulkan::update_uniform_buffer(uint32_t img_index)
{
	void*		data;
	float 		time;
	static auto	start_time = std::chrono::high_resolution_clock::now();
	auto		current_time = std::chrono::high_resolution_clock::now();

	time = std::chrono::duration<float,
		 std::chrono::seconds::period>(current_time - start_time).count();

	//m_ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_ubo.proj = glm::perspective(glm::radians(45.0f)
			, m_swap_chain_extent.width / (float)m_swap_chain_extent.height
			, 0.1f, 10000.0f);

	m_ubo.proj[1][1] *= -1;
	vkMapMemory(m_device, m_uniform_buffer_memory[img_index], 0
			, sizeof(m_ubo), 0, &data);
	memcpy(data, &m_ubo, sizeof(m_ubo));
	vkUnmapMemory(m_device, m_uniform_buffer_memory[img_index]);
	return (0);
}

int		My_vulkan::init()
{
	if (create_instance() == -1)
		return (-1);
	if (create_surface() == -1)
		return (-1);
	if (physical_device() == -1)
		return (-1);
	if (logical_device() == -1)
		return (-1);
	if (swap_chain() == -1)
		return (-1);
	if (image_view() == -1)
		return (-1);
	if (render_pass_create() == -1)
		return (-1);
	if (create_desc_layout() == -1)
		return (-1);
	if (gpu_pipeline_create() == -1)
		return (-1);
	if (command_pool_create() == -1)
		return (-1);
	if (create_depth_resource() == -1)
		return (-1);
	if (framebuffer_create() == -1)
		return (-1);
	if (create_texture_img() == -1)
		return (-1);
	if (create_texture_image_view() == -1)
		return (-1);
	if (create_texture_sampler() == -1)
		return (-1);
	if (create_uniform_buffer() == -1)
		return (-1);
	if (create_vertex_buffer() == -1)
		return (-1);
	if (create_vertex_index_buffer() == -1)
		return (-1);
	if (create_desc_pool() == -1)
		return (-1);
	if (create_desc_set() == -1)
		return (-1);
	if (command_buffer_create() == -1)
		return (-1);
	if (semaphore_create() == -1)
		return (-1);
	return (0);
}

int		My_vulkan::create_uniform_buffer()
{
	VkDeviceSize	buffer_size = sizeof(s_ubo);

	m_uniform_buffer.resize(m_swap_chain_img.size());
	m_uniform_buffer_memory.resize(m_swap_chain_img.size());
	for (size_t i = 0; i < m_swap_chain_img.size(); i++)
	{
		if (create_buffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, m_uniform_buffer[i], m_uniform_buffer_memory[i]) == -1)
		{
			return (-1);
		}
	}
	return (0);
}

int		My_vulkan::create_texture_img()
{
	int						w, h, channel;
	stbi_uc*				pix_buffer;
	VkBuffer				staging_buffer;
	VkDeviceMemory			staging_buffer_memory;

	pix_buffer = stbi_load("textures/block/chiseled_stone_bricks.png", &w, &h
		, &channel, STBI_rgb_alpha);
	VkDeviceSize image_size = w * h * 4;
	if (!pix_buffer)
	{
		std::cout << "Unable to load the texture." << std::endl;
		return (-1);
	}
	if (create_buffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer
		, staging_buffer_memory) == -1)
	{
		std::cout << "Failed to create texture buffer !" << std::endl;
		return (-1);
	}
	void* data;
	vkMapMemory(m_device, staging_buffer_memory, 0, image_size, 0, &data);
	memcpy(data, pix_buffer, static_cast<size_t>(image_size));
	vkUnmapMemory(m_device, staging_buffer_memory);
	stbi_image_free(pix_buffer);
	if (create_img_buffer(w, h, VK_FORMAT_R8G8B8A8_UNORM
		, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT
		| VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_texture_img, m_texture_img_memory) == -1)
	{
		return (-1);
	}
	if (transition_image_layout(m_texture_img, VK_FORMAT_R8G8B8A8_UNORM
		, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) == -1)
		return (-1);
	if (copy_buffer_to_image(staging_buffer, m_texture_img
		, static_cast<uint32_t>(w), static_cast<uint32_t>(h)) == -1)
		return (-1);
	if (transition_image_layout(m_texture_img, VK_FORMAT_R8G8B8A8_UNORM
		, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) == -1)
		return (-1);
	vkDestroyBuffer(m_device, staging_buffer, nullptr);
	vkFreeMemory(m_device, staging_buffer_memory, nullptr);
	return (0);
}

int		My_vulkan::create_texture_sampler()
{
	VkSamplerCreateInfo sampler_info = {};

	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.anisotropyEnable = VK_FALSE;
	sampler_info.maxAnisotropy = 1;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = 0.0f;
	if (vkCreateSampler(m_device, &sampler_info, nullptr
		, &m_texture_sampler) != VK_SUCCESS)
	{
		//throw std::runtime_error("failed to create texture samplerh!");
		return (-1);
	}
	return (0);
}

int		My_vulkan::create_img_buffer(uint32_t width, uint32_t height
			, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage
			, VkMemoryPropertyFlags properties, VkImage& image
			, VkDeviceMemory& image_memory)
{
	VkImageCreateInfo		image_info = {};
	VkMemoryRequirements	mem_requirements;
	VkMemoryAllocateInfo	alloc_info = {};

	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.format = format;
	image_info.tiling = tiling;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = usage;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(m_device, &image_info, nullptr, &image) != VK_SUCCESS)
	{
		return (-1);
	}
	vkGetImageMemoryRequirements(m_device, image, &mem_requirements);
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = 
		find_memory_type(mem_requirements.memoryTypeBits
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(m_device, &alloc_info, nullptr
		, &image_memory) != VK_SUCCESS)
	{
		return (-1);
	}
	vkBindImageMemory(m_device, image, image_memory, 0);
	return (0);
}

int		My_vulkan::create_vertex_buffer()
{
	VkDeviceSize	buffer_size;
	VkBuffer		staging_buffer;
	VkDeviceMemory	staging_buffer_memory;

	buffer_size = sizeof(m_mesh.vertex_buffer[0]) * m_mesh.vertex_buffer.size();
	if (create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, staging_buffer, staging_buffer_memory) == -1)
	{
		return (-1);
	}
	void* data;
	vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, m_mesh.vertex_buffer.data(), (size_t)buffer_size);
	vkUnmapMemory(m_device, staging_buffer_memory);
	if (create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT
		| VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_vertex_buffer, m_vertex_buffer_memory) == -1)
	{
		return (-1);
	}
	copy_buffer(staging_buffer, m_vertex_buffer, buffer_size);
	vkDestroyBuffer(m_device, staging_buffer, nullptr);
	vkFreeMemory(m_device, staging_buffer_memory, nullptr);
	return (0);
}

int		My_vulkan::create_vertex_index_buffer()
{
	VkDeviceSize	buffer_size;
	VkBuffer		staging_buffer;
	VkDeviceMemory	staging_buffer_memory;

	buffer_size = sizeof(m_mesh.index_buffer[0]) * m_mesh.index_buffer.size();
	if (create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, staging_buffer, staging_buffer_memory) == -1)
	{
		return (-1);
	}
	void* data;
	vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, m_mesh.index_buffer.data(), (size_t) buffer_size);
	vkUnmapMemory(m_device, staging_buffer_memory);
	if (create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT
		| VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_vertex_index_buffer, m_vertex_index_buffer_memory) == -1)
	{
		return (-1);
	}
	copy_buffer(staging_buffer, m_vertex_index_buffer, buffer_size);
	vkDestroyBuffer(m_device, staging_buffer, nullptr);
	vkFreeMemory(m_device, staging_buffer_memory, nullptr);
	return (0);
}

int		My_vulkan::create_depth_resource()
{
	VkFormat	depth_format = find_depth_format();

	if (create_img_buffer(m_swap_chain_extent.width, m_swap_chain_extent.height
		, depth_format, VK_IMAGE_TILING_OPTIMAL
		, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
		, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_depth_img, m_depth_img_memory) == -1)
		return (-1);
	if (create_image_view(&m_depth_img_view, m_depth_img, depth_format
		, VK_IMAGE_ASPECT_DEPTH_BIT) == -1)
		return (-1);
	if (transition_image_layout(m_depth_img, depth_format
		, VK_IMAGE_LAYOUT_UNDEFINED
		, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) == -1)
		return (-1);
	return (0);
}

int		My_vulkan::create_desc_layout()
{
	VkDescriptorSetLayoutCreateInfo		layout_info = {};
	static VkDescriptorSetLayoutBinding	ubo_layout_binding = {};
	static VkDescriptorSetLayoutBinding	sampler_layout_binding = {};

	ubo_layout_binding.binding = 0;
	ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_layout_binding.descriptorCount = 1;
	ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	ubo_layout_binding.pImmutableSamplers = NULL;
	sampler_layout_binding.binding = 1;
	sampler_layout_binding.descriptorCount = 1;
	sampler_layout_binding.descriptorType =
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_layout_binding.pImmutableSamplers = nullptr;
	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2>	bindings =
		{ubo_layout_binding, sampler_layout_binding};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
	layout_info.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(m_device, &layout_info, NULL
			, &m_desc_set_layout) != VK_SUCCESS)
	{
		printf("failed to create ubo descriptor layout\n");
		return (-1);
	}
	return (0);
}

int			My_vulkan::create_desc_pool()
{
	VkDescriptorPoolCreateInfo			pool_info = {};
	std::array<VkDescriptorPoolSize, 2>	pool_size = {};

	pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size[0].descriptorCount =
		static_cast<uint32_t>(m_swap_chain_img.size());
	pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_size[1].descriptorCount =
		static_cast<uint32_t>(m_swap_chain_img.size());

	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = static_cast<uint32_t>(pool_size.size());
	pool_info.pPoolSizes = pool_size.data();
	pool_info.maxSets = static_cast<uint32_t>(m_swap_chain_img.size());
	if (vkCreateDescriptorPool(m_device, &pool_info, NULL
			, &m_desc_pool) != VK_SUCCESS)
	{
		printf("failed to create descriptor pool\n");
		return (-1);
	}

	return (0);
}

int			My_vulkan::create_desc_set()
{
	VkDescriptorSetAllocateInfo	alloc_info = {};
	VkDescriptorBufferInfo		buffer_info = {};
	VkDescriptorImageInfo		image_info = {};
	std::array<VkWriteDescriptorSet, 2>
								desc_write = {};
	std::vector<VkDescriptorSetLayout>
								layout(m_swap_chain_img.size()
										, m_desc_set_layout);

	m_desc_set.resize(m_swap_chain_img.size());
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = m_desc_pool;
	alloc_info.descriptorSetCount = m_desc_set.size();
	alloc_info.pSetLayouts = layout.data();
	if (vkAllocateDescriptorSets(m_device, &alloc_info, m_desc_set.data())
		!= VK_SUCCESS)
	{
		printf("failed to allocate descriptor sets!\n");
		return (-1);
	}
	for (int i = 0; i < m_swap_chain_img.size(); i++)
	{
		buffer_info.buffer = m_uniform_buffer[i];
		buffer_info.offset = 0;
		buffer_info.range = sizeof(s_ubo);
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = m_texture_img_view;
		image_info.sampler = m_texture_sampler;

		desc_write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		desc_write[0].dstSet = m_desc_set[i];
		desc_write[0].dstBinding = 0;
		desc_write[0].dstArrayElement = 0;
		desc_write[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		desc_write[0].descriptorCount = 1;
		desc_write[0].pBufferInfo = &buffer_info;
		desc_write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		desc_write[1].dstSet = m_desc_set[i];
		desc_write[1].dstBinding = 1;
		desc_write[1].dstArrayElement = 0;
		desc_write[1].descriptorType =
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		desc_write[1].descriptorCount = 1;
		desc_write[1].pImageInfo = &image_info;
		vkUpdateDescriptorSets(m_device
				, static_cast<uint32_t>(desc_write.size())
				, desc_write.data(), 0, nullptr);
	}
	return (0);
}

int			My_vulkan::create_buffer(VkDeviceSize size
			, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties
			, VkBuffer &buffer, VkDeviceMemory &buffer_mem)
{
	VkBufferCreateInfo			buffer_info = {};
	VkMemoryRequirements		mem_requirements;
	VkMemoryAllocateInfo		alloc_info = {};

	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(m_device, &buffer_info, NULL, &buffer)
		!= VK_SUCCESS)
	{
		printf("failed to create buffer!\n");
		return (-1);
	}
	vkGetBufferMemoryRequirements(m_device, buffer, &mem_requirements);
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex =
		find_memory_type(mem_requirements.memoryTypeBits, properties);
	if (vkAllocateMemory(m_device, &alloc_info, NULL, &buffer_mem)
		!= VK_SUCCESS)
	{
		printf("failed to allocate buffer memory!\n");
		return (-1);
	}
	vkBindBufferMemory(m_device, buffer, buffer_mem, 0);
	return (0);
}

int			My_vulkan::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer
			, VkDeviceSize size) 
{
	VkBufferCopy	copy_region = {};
	VkCommandBuffer	command_buffer;

	if (begin_single_time_command(command_buffer) == -1)
		return (-1);
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size = size;
	vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
	return (end_single_time_command(command_buffer));
}

int			My_vulkan::copy_buffer_to_image(VkBuffer buffer, VkImage image
				, uint32_t width, uint32_t height)
{
	VkCommandBuffer		command_buffer;
	VkBufferImageCopy	copy_region = {};

	begin_single_time_command(command_buffer);
	copy_region.bufferOffset = 0;
	copy_region.bufferRowLength = 0;
	copy_region.bufferImageHeight = 0;
	copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy_region.imageSubresource.mipLevel = 0;
	copy_region.imageSubresource.baseArrayLayer = 0;
	copy_region.imageSubresource.layerCount = 1;
	copy_region.imageOffset = {0, 0, 0};
	copy_region.imageExtent = {width, height, 1};
	vkCmdCopyBufferToImage(command_buffer, buffer, image
		, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);
	end_single_time_command(command_buffer);
	return (0);
}

int		My_vulkan::begin_single_time_command(VkCommandBuffer &command_buffer)
{
	VkCommandBufferAllocateInfo	alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = m_command_pool;
	alloc_info.commandBufferCount = 1;

	vkAllocateCommandBuffers(m_device, &alloc_info, &command_buffer); 
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	if (vkBeginCommandBuffer(command_buffer, &begin_info)
			!= VK_SUCCESS)
	{
		printf("failed to begin recording command buffer!\n");
		return (-1);
	}
	return (0);
}

int		My_vulkan::end_single_time_command(VkCommandBuffer &command_buffer)
{
	VkSubmitInfo	submit_info = {};

	vkEndCommandBuffer(command_buffer);
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	vkQueueSubmit(m_queue_graphic, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_queue_graphic);
	vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
	return (0);
}

int		My_vulkan::transition_image_layout(VkImage image, VkFormat format
	, VkImageLayout old_layout, VkImageLayout new_layout) 
{
	VkCommandBuffer			command_buffer;
	VkImageMemoryBarrier	barrier = {};
	VkPipelineStageFlags	src_stage;
	VkPipelineStageFlags	dst_stage;

	begin_single_time_command(command_buffer);

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED
		&& new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		&& new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED
		&& new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
			| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;  
	}
	else
		return (-1);
	vkCmdPipelineBarrier(command_buffer, src_stage, dst_stage
		, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	end_single_time_command(command_buffer);
	return (0);
}

int		My_vulkan::create_instance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Basic Vbo";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	uint32_t	val_layer_count;
	char		*val_layer[1] = {"VK_LAYER_LUNARG_standard_validation"};
	uint32_t	ext_count;
	char		**ext;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	val_layer_count = 1;
	ext = get_extensions(m_debug, &ext_count);
	if (ext == NULL && ext_count > 0)
	{
		printf("failed to get extensions!\n");
		return (-1);
	}
	createInfo.enabledExtensionCount = ext_count;
	createInfo.ppEnabledExtensionNames = ext;
	createInfo.enabledLayerCount = 0;
	if (m_debug == 1)
	{
		if (!val_layer_check(val_layer, val_layer_count))
		{
			printf("no corresponding validation layer found\n");
			return (-1);
		}
		createInfo.enabledLayerCount = val_layer_count;
		createInfo.ppEnabledLayerNames = val_layer;
	}

	if (vkCreateInstance(&createInfo, NULL, &m_instance) != VK_SUCCESS)
	{
		printf("failed to create instance!\n");
		return (-1);
	}
	return (0);
}

int		My_vulkan::render_pass_create()
{
	static VkAttachmentDescription	color_attachment = {};
	static VkAttachmentReference	color_attachment_ref = {};
	static VkAttachmentDescription	depth_attachment = {};
	static VkAttachmentReference	depth_attachment_ref = {};
	static VkSubpassDescription		subpass = {};
	static VkSubpassDependency		subpass_dep = {};
	VkRenderPassCreateInfo			render_pass_info = {};

	color_attachment.format = m_swap_chain_img_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	depth_attachment.format = find_depth_format();
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout =
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout =
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;
	subpass_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dep.dstSubpass = 0;
	subpass_dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dep.srcAccessMask = 0;
	subpass_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
		| VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	std::array<VkAttachmentDescription, 2> attachments =
		{color_attachment, depth_attachment};
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &subpass_dep;
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount =
		static_cast<uint32_t>(attachments.size());
	render_pass_info.pAttachments = attachments.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	if (vkCreateRenderPass(m_device, &render_pass_info, NULL
			, &m_render_pass) != VK_SUCCESS)
	{
		printf("failed ton create render pass!\n");
		return (-1);
	}
	return (0);
}

int		My_vulkan::queue_family(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	int						i;
	uint32_t				queue_count;
	VkQueueFamilyProperties	*queue;
	VkBool32				present_support;

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, NULL);
	if (queue_count <= 0 || (queue = (VkQueueFamilyProperties*)
			malloc(sizeof(*queue) * queue_count)) == NULL)
		return (-1);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, queue);
	i = 0;
	while (i < queue_count)
	{
		present_support = 0;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface
			, &present_support);
		if (queue[i].queueCount > 0 && queue[i].queueFlags
			& VK_QUEUE_GRAPHICS_BIT && present_support)
			return (i);
		i++;
	}
	return (-1);
}


int		My_vulkan::is_device_suitable(VkPhysicalDevice device,
	VkSurfaceKHR surface)
{
	VkPhysicalDeviceProperties	dev_prop;
	VkPhysicalDeviceFeatures	dev_feat;
	s_swap_chain_detail			detail;

	vkGetPhysicalDeviceProperties(device, &dev_prop);
	vkGetPhysicalDeviceFeatures(device, &dev_feat);
	if (queue_family(device, surface) == -1)
		return (0);
	detail = swap_get_detail(device, surface);
	if (detail.format == NULL || detail.present_mode == NULL)
		return (0);
	return (1);
}

#include <string.h>

int		My_vulkan::logical_device()
{
	int							dev_i;
	float						queue_priority;
	VkDeviceCreateInfo			dev_inf = {};
	VkDeviceQueueCreateInfo		queue_inf = {};
	VkPhysicalDeviceFeatures	dev_feat = {};
	char						*ext_name;

	ext_name = strdup(VK_KHR_SWAPCHAIN_EXTENSION_NAME); //libc!! free??
	if ((dev_i = queue_family(m_dev_phy, m_surface)) == -1)
		return (-1);
	queue_inf.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_inf.queueFamilyIndex = dev_i;
	queue_inf.queueCount = 1;
	queue_priority = 1.0f;
	queue_inf.pQueuePriorities = &queue_priority;
	dev_inf.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dev_inf.pQueueCreateInfos = &queue_inf;
	dev_inf.queueCreateInfoCount = 1;
	dev_inf.pEnabledFeatures = &dev_feat;
	dev_inf.enabledExtensionCount = 1;
	dev_inf.ppEnabledExtensionNames = &ext_name;
	dev_inf.enabledLayerCount = 0;
	if (vkCreateDevice(m_dev_phy, &dev_inf, NULL, &m_device) != VK_SUCCESS)
	{
		printf("failed to create logical device!\n");
		return (-1);
	}
	vkGetDeviceQueue(m_device, dev_i, 0, &m_queue_graphic);
	return (0);
}

int		My_vulkan::physical_device()
{
	int					i;
	uint32_t			device_count;
	VkPhysicalDevice	*device;

	vkEnumeratePhysicalDevices(m_instance, &device_count, NULL);
	if (device_count == 0)
	{
		printf("failed to find GPU with Vulkan support!\n");
		return (-1);
	}
	if ((device = (VkPhysicalDevice*)
		malloc(sizeof(*device) * device_count)) == NULL)
		return (-1);
	vkEnumeratePhysicalDevices(m_instance, &device_count, device);
	i = 0;
	m_dev_phy = VK_NULL_HANDLE;
	while (i < device_count && m_dev_phy == NULL)
	{
		if(is_device_suitable(device[i], m_surface))
			m_dev_phy = device[i];
		i++;
	}
	if (m_dev_phy == VK_NULL_HANDLE)
	{
		printf("failed to find a suitable GPU!\n");
		return (-1);
	}
	return (0);
}

int		My_vulkan::create_surface()
{
	if (glfwCreateWindowSurface(m_instance, m_win, NULL, &m_surface)
		!= VK_SUCCESS)
	{
		printf("failed to create window surface!\n");
		return (-1);
	}
	return (0);
}

int		My_vulkan::swap_chain()
{
	s_swap_chain_detail			detail;
	VkSurfaceFormatKHR			format;
	VkPresentModeKHR			present_mode;
	VkExtent2D					extent;
	uint32_t					img_count;
	VkSwapchainCreateInfoKHR	swap_info = {};

	detail = swap_get_detail(m_dev_phy, m_surface);
	format = swap_choose_format(detail.format);
	present_mode = swap_choose_present_mode(detail.present_mode);
	extent = swap_choose_extent(detail.capability);
	img_count = detail.capability.minImageCount + 1;
	if (detail.capability.maxImageCount > 0
		&& img_count > detail.capability.maxImageCount)
		img_count = detail.capability.maxImageCount;
	swap_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swap_info.surface = m_surface;
	swap_info.minImageCount = img_count;
	swap_info.imageFormat = format.format;
	swap_info.imageColorSpace = format.colorSpace;
	swap_info.imageExtent = extent;
	swap_info.imageArrayLayers = 1;
	swap_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swap_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swap_info.queueFamilyIndexCount = 0;
	swap_info.pQueueFamilyIndices = NULL;
	swap_info.preTransform = detail.capability.currentTransform;
	swap_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swap_info.presentMode = present_mode;
	swap_info.clipped = VK_TRUE;
	swap_info.oldSwapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(m_device, &swap_info, NULL, &m_swap_chain)
		!= VK_SUCCESS)
	{
		printf("failed to create swap chain!\n");
		return (-1);
	}
	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &img_count, NULL);
	if (img_count <= 0)
		return (-1);
	m_swap_chain_img.resize(img_count);
	vkGetSwapchainImagesKHR(m_device, m_swap_chain
			, &img_count, m_swap_chain_img.data());
	m_swap_chain_img_format = format.format;
	m_swap_chain_extent = extent;
	return (0);
}

int		My_vulkan::image_view()
{
	int						i;
	uint32_t				img_count;

	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &img_count, NULL);
	m_image_view.resize(img_count);
	i = 0;
	while (i < img_count)
	{
		if (create_image_view(&m_image_view[i], m_swap_chain_img[i]
			, m_swap_chain_img_format, VK_IMAGE_ASPECT_COLOR_BIT) == -1)
			return (-1);
		i++;
	}
	return (0);
}

int		My_vulkan::create_texture_image_view()
{
	if (create_image_view(&m_texture_img_view, m_texture_img
		, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT) == -1)
		return (-1);
	return (0);
}

int		My_vulkan::create_image_view(VkImageView *img_view, VkImage &image
			, VkFormat format, VkImageAspectFlags aspect_flags)
{
	VkImageViewCreateInfo	view_info = {};

	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;
	view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.subresourceRange.aspectMask = aspect_flags;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	if (vkCreateImageView(m_device, &view_info, NULL
				, img_view) != VK_SUCCESS)
	{
		printf("failed to create image views!");
		return (-1);
	}
	return (0);
}

int		My_vulkan::framebuffer_create()
{
	int						i;
	uint32_t				img_count;
	VkFramebufferCreateInfo	framebuffer_info;

	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &img_count, NULL);
	if ((m_framebuffer = (VkFramebuffer*)
		malloc(sizeof(VkFramebuffer) * img_count)) == NULL)
		return (-1);
	i = 0;
	framebuffer_info = (VkFramebufferCreateInfo){};
	while (i < img_count)
	{
		std::array<VkImageView, 2> attachments =
			{m_image_view[i], m_depth_img_view};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = m_render_pass;
		framebuffer_info.attachmentCount =
			static_cast<uint32_t>(attachments.size());
		framebuffer_info.pAttachments = attachments.data();
		framebuffer_info.width = m_swap_chain_extent.width;
		framebuffer_info.height = m_swap_chain_extent.height;
		framebuffer_info.layers = 1;
		if (vkCreateFramebuffer(m_device, &framebuffer_info, NULL
				, &m_framebuffer[i]) != VK_SUCCESS)
		{
			printf("failed to create framebuffer!\n");
			return (-1);
		}
		i++;
	}
	return (0);
}

int		My_vulkan::command_pool_create()
{
	int						queue_indice;
	VkCommandPoolCreateInfo	pool_info;

	queue_indice = queue_family(m_dev_phy, m_surface);
	pool_info = (VkCommandPoolCreateInfo){};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = queue_indice;
	pool_info.flags = 0;
	if (vkCreateCommandPool(m_device, &pool_info, NULL
			, &m_command_pool) != VK_SUCCESS)
	{
		printf("failed to create command pool!\n");
		return (-1);
	}
	return (0);
}

int		My_vulkan::command_buffer_create()
{
	int							i;
	uint32_t					img_count;
	VkCommandBufferAllocateInfo alloc_info;
	VkCommandBufferBeginInfo	begin_info;
	VkRenderPassBeginInfo		render_pass_info;
	VkClearValue				clear_color;
	std::array<VkClearValue, 2> clear_value = {};

	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &img_count, NULL);
	if ((m_command_buffer = (VkCommandBuffer*)
				malloc(sizeof(VkCommandBuffer) * img_count)) == NULL)
		return (-1);
	alloc_info = (VkCommandBufferAllocateInfo){};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = m_command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = img_count;
	if (vkAllocateCommandBuffers(m_device, &alloc_info
			, m_command_buffer) != VK_SUCCESS)
	{
		printf("failed to create allocate buffer!\n");
		return (-1);
	}
	i = 0;
	begin_info = (VkCommandBufferBeginInfo){};
	clear_value[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
	clear_value[1].depthStencil = {1.0f, 0};
	while (i < img_count)
	{
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		begin_info.pInheritanceInfo = NULL;
		if (vkBeginCommandBuffer(m_command_buffer[i], &begin_info)
			!= VK_SUCCESS)
		{
			printf("failed to begin recording command buffer!\n");
			return (-1);
		}
		render_pass_info = (VkRenderPassBeginInfo){};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_render_pass;
		render_pass_info.framebuffer = m_framebuffer[i];
		render_pass_info.renderArea.offset = (VkOffset2D){0, 0};
		render_pass_info.renderArea.extent = m_swap_chain_extent;
		render_pass_info.clearValueCount =
			static_cast<uint32_t>(clear_value.size());
		render_pass_info.pClearValues = clear_value.data();
		vkCmdBeginRenderPass(m_command_buffer[i], &render_pass_info
			, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_command_buffer[i]
			, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
		VkBuffer vertex_buffers[] = {m_vertex_buffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(m_command_buffer[i], 0, 1
		, (const VkBuffer*)vertex_buffers, offsets);
		vkCmdBindIndexBuffer(m_command_buffer[i], m_vertex_index_buffer, 0
				, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(m_command_buffer[i]
				, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout
				, 0, 1, &m_desc_set[i], 0, nullptr);
		vkCmdDrawIndexed(m_command_buffer[i]
			, static_cast<uint32_t>(m_mesh.index_buffer.size()), 1, 0, 0, 0);
		vkCmdEndRenderPass(m_command_buffer[i]);
		if (vkEndCommandBuffer(m_command_buffer[i]) != VK_SUCCESS)
		{
			printf("failed to record command buffer!\n");
			return (-1);
		}
		i++;
	}
	return (0);
}

int		My_vulkan::gpu_pipeline_create()
{
	char				*vert_shader;
	char				*frag_shader;
	int					vert_size;
	int					frag_size;
	VkShaderModule		vert_shader_module;
	VkShaderModule		frag_shader_module;
	VkPipelineLayoutCreateInfo		pipeline_layout_info;
	VkGraphicsPipelineCreateInfo	pipeline_info;

	VkPipelineRasterizationStateCreateInfo	rasterizer_info;
	VkPipelineViewportStateCreateInfo		viewport_info;
	VkPipelineInputAssemblyStateCreateInfo	input_assembly;
	VkPipelineColorBlendStateCreateInfo		color_blend_info;
	VkPipelineMultisampleStateCreateInfo	multisampling_info;
	VkPipelineVertexInputStateCreateInfo	vert_input_info;
	VkPipelineShaderStageCreateInfo			*shader_stage;
	VkPipelineDepthStencilStateCreateInfo	depth_stencil_info;

	vert_shader = shader_load("shader/vert.spv", &vert_size);
	frag_shader = shader_load("shader/frag.spv", &frag_size);
	printf("shader size (%d, %d)\n", vert_size, frag_size);
	if (shader_create_module(m_device, vert_shader, vert_size
		, &vert_shader_module) == -1)
		return (-1);
	if (shader_create_module(m_device, frag_shader, frag_size
		, &frag_shader_module) == -1)
		return (-1);

	shader_stage = pipe_shader_create(vert_shader_module, frag_shader_module);
	if (shader_stage == NULL)
		return (-1);
	vert_input_info = vert_input_create();
	input_assembly = input_assembly_create();
	viewport_info = viewport_create(m_swap_chain_extent);
	rasterizer_info = rasterizer_create();
	multisampling_info = multisampling_create();
	color_blend_info = color_blend_create();
	depth_stencil_info = depth_stencil_create();

	pipeline_layout_info = (VkPipelineLayoutCreateInfo){};
	pipeline_layout_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 1;
	pipeline_layout_info.pSetLayouts = &m_desc_set_layout;
	if (vkCreatePipelineLayout(m_device, &pipeline_layout_info, NULL
		, &m_pipeline_layout) != VK_SUCCESS)
	{
		printf("failed to create pipeline layout!\n");
		return (-1);
	}
	pipeline_info = (VkGraphicsPipelineCreateInfo){};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stage;
	pipeline_info.pVertexInputState = &vert_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pDepthStencilState = &depth_stencil_info;
	pipeline_info.pColorBlendState = &color_blend_info;
	pipeline_info.pDynamicState = NULL;
	pipeline_info.layout = m_pipeline_layout;
	pipeline_info.renderPass = m_render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = -1;
	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1
			, &pipeline_info, NULL, &m_graphics_pipeline) != VK_SUCCESS)
	{
		printf("failed to create graphics pipeline!");
		return (-1);
	}

	vkDestroyShaderModule(m_device, frag_shader_module, NULL);
	vkDestroyShaderModule(m_device, vert_shader_module, NULL);
	return (0);
}

int		My_vulkan::semaphore_create()
{
	VkSemaphoreCreateInfo	semaphore_info;

	semaphore_info = (VkSemaphoreCreateInfo){};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(m_device, &semaphore_info, NULL
			, &m_semaphore_image_available) != VK_SUCCESS
			|| vkCreateSemaphore(m_device, &semaphore_info, NULL
			, &m_semaphore_render_finish) != VK_SUCCESS)
	{
		printf("failed to create semaphore!\n");
		return (-1);
	}
	return (0);
}

ssize_t	My_vulkan::find_memory_type(uint32_t typeFilter,
	VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties	mem_properties;

	vkGetPhysicalDeviceMemoryProperties(m_dev_phy, &mem_properties);
	for (ssize_t i = 0; i < mem_properties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && 
			(mem_properties.memoryTypes[i].propertyFlags & properties)
			== properties) 
			return (i);
	}
	return (-1);
}

VkFormat	My_vulkan::find_depth_format()
{
	return find_supported_format({VK_FORMAT_D32_SFLOAT
			, VK_FORMAT_D32_SFLOAT_S8_UINT
			, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL
		, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat	My_vulkan::find_supported_format(const std::vector<VkFormat>
			&candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	VkFormatProperties props;

	for (VkFormat format : candidates)
	{
		vkGetPhysicalDeviceFormatProperties(m_dev_phy, format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR 
			&& (props.linearTilingFeatures & features) == features)
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
				(props.optimalTilingFeatures & features) == features)
			return format;
	}
	throw std::runtime_error("failed to find supported format!");
}

bool		has_stencil_component(VkFormat format)
{
	return (format == VK_FORMAT_D32_SFLOAT_S8_UINT
			|| format == VK_FORMAT_D24_UNORM_S8_UINT);
}
