/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   My_vulkan.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/09 17:05:37 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/25 16:59:06 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Mesh.h"
#include <vector>
#include <unordered_map>
#define GLFW_INCLUDE_VULKAN
#define	ALLOC_VERTEX_BUF_SIZE	(15000000)
#define	ALLOC_INDEX_BUF_SIZE	(30000000)
#include "GLFW/glfw3.h"

#define	MAX_FRAME_IN_FLIGHT	2

struct s_ubo
{
	glm::mat4	model;
	glm::mat4	view;
	glm::mat4	proj;
	glm::vec3	sun_pos;
};

class	My_vulkan;

struct	s_chunk
{
	s_chunk(Moore_accessor &moore_access): mesh(moore_access){};
	~s_chunk()
	{
		/*
		vkDestroyBuffer(device_ref, m_vertex_index_buffer, nullptr);
		vkFreeMemory(device_ref, m_vertex_index_buffer_memory, nullptr);
		vkDestroyBuffer(device_ref, m_vertex_buffer, nullptr);
		vkFreeMemory(device_ref, m_vertex_buffer_memory, nullptr);
		*/;
	}
	int			update(My_vulkan &vulk);
	void		command_buffer_binder(VkCommandBuffer &cmd_buffer);
	void		command_buffer_binder_blend(VkCommandBuffer &cmd_buffer);
	void		reset(My_vulkan &vulk);
	void		unload(My_vulkan &vulk);
	int			alloc_buffer(My_vulkan &vulk, VkDeviceSize vbo_size
					, VkDeviceSize ibo_size, VkDeviceSize blend_size);

	Mesh<s_vertex>	mesh;
	bool			in_vbo;
	bool			need_remesh = false;
	bool			need_unload = false;
	bool			has_unload = false;
	s_vec3i			origin;
	bool			has_blend = false;

	private:
		VkBuffer		m_vertex_buffer;
		VkDeviceMemory	m_vertex_buffer_memory;
		VkBuffer		m_vertex_index_buffer;
		VkDeviceMemory	m_vertex_index_buffer_memory;
		VkBuffer		m_index_buffer_blend;
		VkDeviceMemory	m_index_buffer_blend_memory;
};

struct	s_enemy
{
	s_enemy(){};
	~s_enemy()
	{
		/*
		vkDestroyBuffer(device_ref, m_vertex_index_buffer, nullptr);
		vkFreeMemory(device_ref, m_vertex_index_buffer_memory, nullptr);
		vkDestroyBuffer(device_ref, m_vertex_buffer, nullptr);
		vkFreeMemory(device_ref, m_vertex_buffer_memory, nullptr);
		*/;
	}
	int			update(My_vulkan &vulk, Mesh<s_vertex_bones> &mesh);
	void		command_buffer_binder(VkCommandBuffer &cmd_buffer);
	void		command_buffer_binder_blend(VkCommandBuffer &cmd_buffer);
	void		unload(My_vulkan &vulk);
	int			alloc_buffer(My_vulkan &vulk, VkDeviceSize vbo_size
					, VkDeviceSize ibo_size);

	bool			in_vbo;
	bool			need_remesh = false;
	bool			need_unload = false;
	bool			has_unload = false;
	int				nb_index;
	s_vec3i			origin;

	private:
		VkBuffer		m_vertex_buffer;
		VkDeviceMemory	m_vertex_buffer_memory;
		VkBuffer		m_vertex_index_buffer;
		VkDeviceMemory	m_vertex_index_buffer_memory;
};

using ChunkKey = std::pair<uint32_t, uint32_t>;

struct Key_hash
{
	size_t operator()(const ChunkKey& key) const
	{
		return (key.first | (key.second << 10));
	}
};

using t_chunk_cont = std::unordered_map<ChunkKey, s_chunk, Key_hash>;
using t_enemy_cont = s_enemy;

class	My_vulkan
{
	public:
		My_vulkan(GLFWwindow *win, s_ubo &ubo);
		~My_vulkan();

		int			init();
		int			draw_frame();
		int			update_uniform_buffer(uint32_t img_index);
		const VkDevice
					&get_device_ref() const {return (const VkDevice&)(m_device);}
		int			command_buffer_record(t_chunk_cont &chunk_vec);
		int			command_buffer_record(t_enemy_cont &chunk_vec);
		int			create_buffer(VkDeviceSize size, VkBufferUsageFlags usage
						, VkMemoryPropertyFlags properties, VkBuffer &buffer
						, VkDeviceMemory &buffer_mem);
		int			copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer
						, VkDeviceSize size
						, uint32_t src_offset = 0, uint32_t dst_offset = 0); 
		int			begin_single_time_command(VkCommandBuffer &command_buffer);
		int			end_single_time_command(VkCommandBuffer &command_buffer);
		int			copy_staging_to_buffer(VkBuffer dst
						, VkBuffer &staging_buffer
						, VkDeviceMemory &staging_buffer_memory
						, VkDeviceSize copy_size, uint32_t offset = 0);
		/*
		int			copy_vertex_buffer();
		int			copy_vertex_index_buffer();
		*/

	private:
		int			create_uniform_buffer();
		int			create_texture_img();
		int			create_texture_sampler();
		int			create_vertex_buffer();
		int			create_vertex_index_buffer();
		int			create_depth_resource();
		int			create_desc_layout();
		int			create_desc_set();
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
		int			create_texture_image_view();
		int			create_image_view(VkImageView *img_view, VkImage &image
						, VkFormat format, VkImageAspectFlags aspectFlags);
		int			framebuffer_create();
		int			command_pool_create();
		int			command_buffer_create();
		int			gpu_pipeline_create();
		int			semaphore_create();
		int			create_img_buffer(uint32_t width, uint32_t height
						, VkFormat format, VkImageTiling tiling
						, VkImageUsageFlags usage
						, VkMemoryPropertyFlags properties, VkImage& image
						, VkDeviceMemory& image_memory);
		int			copy_buffer_to_image(VkBuffer buffer, VkImage image
						, uint32_t width, uint32_t height);
		int			transition_image_layout(VkImage image, VkFormat format
						, VkImageLayout old_layout, VkImageLayout new_layout);
		ssize_t		find_memory_type(uint32_t typeFilter
						, VkMemoryPropertyFlags properties);
		VkFormat	find_depth_format();
		VkFormat	find_supported_format(const std::vector<VkFormat>& candidate
						, VkImageTiling tiling, VkFormatFeatureFlags features);
		bool		has_stencil_component(VkFormat format);

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
		/*
		VkBuffer				m_vertex_buffer;
		VkDeviceMemory			m_vertex_buffer_memory;
		VkBuffer				m_vertex_index_buffer;
		VkDeviceMemory			m_vertex_index_buffer_memory;
		*/
		VkDescriptorSetLayout	m_desc_set_layout;
		std::vector<VkDescriptorSet>
								m_desc_set;
		VkDescriptorPool		m_desc_pool;
		VkImage					m_texture_img;
		VkDeviceMemory			m_texture_img_memory;
		VkImageView				m_texture_img_view;
		VkSampler				m_texture_sampler;
		VkImage					m_depth_img;
		VkDeviceMemory			m_depth_img_memory;
		VkImageView				m_depth_img_view;

		int						m_update;
		bool					m_debug = false;
		s_ubo					&m_ubo;
};
