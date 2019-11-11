/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   My_vulkan.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/09 17:05:36 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/11 11:02:04 by trobicho         ###   ########.fr       */
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

My_vulkan::My_vulkan(GLFWwindow *win, Mesh &mesh): m_win(win), m_mesh(mesh) //try catch
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
	vkDestroyDescriptorSetLayout(m_device, m_ubo_desc_set_layout, nullptr);
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

	m_ubo.model = glm::rotate(glm::mat4(1.0f), 0 * glm::radians(90.0f)
			, glm::vec3(0.0f, 0.0f, 1.0f));
	m_ubo.view = glm::lookAt(glm::vec3(10.0f, 70.0f, 80.0f)
			, glm::vec3(0.0f, 0.0f, 0.0f)
			, glm::vec3(0.0f, 1.0f, 0.0f));
	m_ubo.proj = glm::perspective(glm::radians(45.0f)
			, m_swap_chain_extent.width / (float)m_swap_chain_extent.height
			, 0.1f, 10.0f);
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
	if (create_ubo_desc_layout() == -1)
		return (-1);
	if (gpu_pipeline_create() == -1)
		return (-1);
	if (framebuffer_create() == -1)
		return (-1);
	if (command_pool_create() == -1)
		return (-1);
	if (create_uniform_buffer() == -1)
		return (-1);
	if (create_vertex_buffer() == -1)
		return (-1);
	if (create_vertex_index_buffer() == -1)
		return (-1);
	if (create_desc_pool() == -1)
		return (-1);
	if (create_ubo_desc_set() == -1)
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
	memcpy(data, m_mesh.vertex_buffer.data(), (size_t) buffer_size); 
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

int			My_vulkan::create_ubo_desc_layout()
{
	VkDescriptorSetLayoutCreateInfo		layout_info = {};
	static VkDescriptorSetLayoutBinding	layout_binding = {};

	layout_binding.binding = 0;
	layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layout_binding.descriptorCount = 1;
	layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layout_binding.pImmutableSamplers = NULL;
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = 1;
	layout_info.pBindings = &layout_binding;
	if (vkCreateDescriptorSetLayout(m_device, &layout_info, NULL
			, &m_ubo_desc_set_layout) != VK_SUCCESS)
	{
		printf("failed to create ubo descriptor layout\n");
		return (-1);
	}
	return (0);
}

int			My_vulkan::create_desc_pool()
{
	uint32_t					img_count;
	VkDescriptorPoolSize		pool_size;
	VkDescriptorPoolCreateInfo	pool_info;

	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &img_count, NULL);
	pool_info = (VkDescriptorPoolCreateInfo){};
	pool_size = (VkDescriptorPoolSize){};
	pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = img_count;
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = 1;
	pool_info.pPoolSizes = &pool_size;
	pool_info.maxSets = img_count;
	if (vkCreateDescriptorPool(m_device, &pool_info, NULL
			, &m_desc_pool) != VK_SUCCESS)
	{
		printf("failed to create descriptor pool\n");
		return (-1);
	}

	return (0);
}

int			My_vulkan::create_ubo_desc_set()
{
	int							i;
	VkDescriptorSetAllocateInfo	alloc_info;
	std::vector<VkDescriptorSetLayout>
								layout(m_swap_chain_img.size()
									, m_ubo_desc_set_layout);
	VkDescriptorBufferInfo		buffer_info;
	VkWriteDescriptorSet		desc_write;

	buffer_info = (VkDescriptorBufferInfo){};
	desc_write = (VkWriteDescriptorSet){};
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
	i = 0;
	while (i < m_desc_set.size())
	{
		buffer_info.buffer = m_uniform_buffer[i];
		buffer_info.offset = 0;
		buffer_info.range = sizeof(s_ubo);
		desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		desc_write.dstSet = m_desc_set[i];
		desc_write.dstBinding = 0;
		desc_write.dstArrayElement = 0;
		desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		desc_write.descriptorCount = 1;
		desc_write.pBufferInfo = &buffer_info;
		vkUpdateDescriptorSets(m_device, 1, &desc_write, 0, nullptr);
		i++;
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
	VkCommandBufferAllocateInfo	alloc_info = {};
	VkBufferCopy copy_region = {};
	VkSubmitInfo submit_info = {};

	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = m_command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
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
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size = size;
	vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
	vkEndCommandBuffer(command_buffer);
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	vkQueueSubmit(m_queue_graphic, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_queue_graphic);
	vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
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
	static VkAttachmentDescription color_attachment;
	static VkAttachmentReference	color_attachment_ref;
	static VkSubpassDescription	subpass;
	static VkSubpassDependency	subpass_dep;
	VkRenderPassCreateInfo	render_pass_info;

	subpass = (VkSubpassDescription){};
	subpass_dep = (VkSubpassDependency){};
	color_attachment_ref = (VkAttachmentReference){};
	color_attachment = (VkAttachmentDescription){};
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
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dep.dstSubpass = 0;
	subpass_dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dep.srcAccessMask = 0;
	subpass_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
		| VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &subpass_dep;
	render_pass_info = (VkRenderPassCreateInfo){};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
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
	VkImageViewCreateInfo	view_info;

	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &img_count, NULL);
	m_image_view.resize(img_count);
	i = 0;
	view_info = (VkImageViewCreateInfo){};
	while (i < img_count)
	{
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = m_swap_chain_img[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = m_swap_chain_img_format;
		view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		if (vkCreateImageView(m_device, &view_info, NULL
				, &m_image_view[i]) != VK_SUCCESS)
		{
			printf("failed to create image views!");
			return (-1);
		}
		i++;
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
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = m_render_pass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = &m_image_view[i];
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
	clear_color = (VkClearValue){0.0f, 0.0f, 0.0f, 1.0f};
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
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;
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

	pipeline_layout_info = (VkPipelineLayoutCreateInfo){};
	pipeline_layout_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 1;
	pipeline_layout_info.pSetLayouts = &m_ubo_desc_set_layout;
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
	pipeline_info.pDepthStencilState = NULL;
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
