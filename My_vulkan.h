/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   My_vulkan.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/09 17:05:37 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/12 07:07:21 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Mesh.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

struct s_ubo
{
	glm::mat4	model;
	glm::mat4	view;
	glm::mat4	proj;
};

class	My_vulkan
{
	public:
		My_vulkan(GLFWwindow *win, Mesh &mesh, s_ubo &ubo);
		~My_vulkan();

		int			init();
		int			draw_frame();
		int			update_uniform_buffer(uint32_t img_index);
		VkDevice	&get_device_ref(){return (m_device);}

	private:
		int			create_uniform_buffer();
		int			create_vertex_buffer();
		int			create_vertex_index_buffer();
		int			create_ubo_desc_layout();
		int			create_ubo_desc_set();
		int			create_desc_pool();
		int			create_instance();
		int			render_pass_create();
		int			queue_family(VkPhysicalDevice device, VkSurfaceKHR surface);
		int			is_device_suitable(VkPhysicalDevice device,
			VkSurfaceKHR surface);
		int			logical_device();
		int			physical_device();
		int			create_surface();
		int			swap_chain();
		int			image_view();
		int			framebuffer_create();
		int			command_pool_create();
		int			command_buffer_create();
		int			gpu_pipeline_create();
		int			semaphore_create();
		int			create_buffer(VkDeviceSize size, VkBufferUsageFlags usage
						, VkMemoryPropertyFlags properties, VkBuffer &buffer
						, VkDeviceMemory &buffer_mem);
		int			copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer
						, VkDeviceSize size);
		ssize_t		find_memory_type(uint32_t typeFilter
						, VkMemoryPropertyFlags properties);

		GLFWwindow				*m_win;
		VkInstance				m_instance;
		VkDevice				m_device;
		VkPhysicalDevice		m_dev_phy;
		VkQueue					m_queue_graphic;
		VkSurfaceKHR			m_surface;
		VkSwapchainKHR			m_swap_chain;
		std::vector<VkImage>	m_swap_chain_img;
		std::vector<VkImageView>
								m_image_view;
		VkFormat				m_swap_chain_img_format;
		VkExtent2D				m_swap_chain_extent;
		VkRenderPass			m_render_pass;
		VkPipelineLayout		m_pipeline_layout;
		VkPipeline				m_graphics_pipeline;
		VkFramebuffer			*m_framebuffer;
		VkCommandPool			m_command_pool;
		VkCommandBuffer			*m_command_buffer;
		VkSemaphore				m_semaphore_image_available; // no multiple semaphore for multiple frame ????
		VkSemaphore				m_semaphore_render_finish;
		std::vector<VkBuffer> 	m_uniform_buffer;
		std::vector<VkDeviceMemory>
								m_uniform_buffer_memory;
		VkBuffer				m_vertex_buffer;
		VkDeviceMemory			m_vertex_buffer_memory;
		VkBuffer				m_vertex_index_buffer;
		VkDeviceMemory			m_vertex_index_buffer_memory;
		VkDescriptorSetLayout	m_ubo_desc_set_layout;
		std::vector<VkDescriptorSet>
								m_desc_set;
		VkDescriptorPool		m_desc_pool;
		int						m_update;
		bool					m_debug = false;
		Mesh					&m_mesh;
		s_ubo					&m_ubo;
};
