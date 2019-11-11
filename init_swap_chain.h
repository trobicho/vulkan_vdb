/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_swap_chain.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/22 12:58:26 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/09 22:37:43 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

struct		s_swap_chain_detail
{
	VkSurfaceCapabilitiesKHR	capability;
	VkSurfaceFormatKHR			*format;
	VkPresentModeKHR			*present_mode;
};

s_swap_chain_detail	swap_get_detail(VkPhysicalDevice device,
	VkSurfaceKHR surface);
VkSurfaceFormatKHR	swap_choose_format(const VkSurfaceFormatKHR *format);
VkPresentModeKHR	swap_choose_present_mode(const VkPresentModeKHR *format);
VkExtent2D			swap_choose_extent(
	const VkSurfaceCapabilitiesKHR capability);
