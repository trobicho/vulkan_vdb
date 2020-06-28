/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dev_gui.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/15 12:55:28 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/19 06:55:23 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "My_vulkan.h"
#include <vector>

class	Dev_gui
{
	public:
		Dev_gui(My_vulkan &vulk): m_vulk(vulk){};
		Dev_gui(My_vulkan &vulk, GLFWwindow *win);

		void	new_frame();

	private:
		void	create_framebuffer(ImGui_ImplVulkan_InitInfo &init_info);
		void	create_desc_pool(ImGui_ImplVulkan_InitInfo &init_info);
		void	create_render_pass(ImGui_ImplVulkan_InitInfo &init_info);

		My_vulkan				&m_vulk;
		GLFWwindow				*m_win;
		VkDescriptorPool		m_desc_pool;
		VkRenderPass			m_render_pass;
		std::vector<VkCommandBuffer>
								m_command_buffer;
		VkCommandPool			m_command_pool;
		std::vector<VkFramebuffer>
								m_framebuffer;

};
