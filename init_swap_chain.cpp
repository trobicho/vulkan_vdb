/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_swap_chain.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/22 12:59:31 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/09 22:41:55 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "init_swap_chain.h"
#include <stdlib.h>
#include <stdint.h>
#include <cmath>

s_swap_chain_detail			swap_get_detail(VkPhysicalDevice device,
	VkSurfaceKHR surface)
{
	s_swap_chain_detail	detail;
	uint32_t			count;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface
		, &detail.capability);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface
		, &count, NULL);
	if (count != 0)
	{
		if ((detail.format = (VkSurfaceFormatKHR*)
					malloc(sizeof(*detail.format) * count)) == NULL)
			return (detail);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface
			, &count, detail.format);
	}
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface
		, &count, NULL);
	if (count != 0)
	{
		if ((detail.present_mode = (VkPresentModeKHR*)
					malloc(sizeof(*detail.present_mode) * count)) == NULL)
			return (detail);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface
			, &count, detail.present_mode);
	}
	return (detail);
}

#include <stdio.h>
VkSurfaceFormatKHR			swap_choose_format(
	const VkSurfaceFormatKHR *format)
{
	int i;
	int	n;

	n = sizeof(format) / sizeof(*format);
	printf("nbFormat: %d\n", n);
	if (n == 1 && format[0].format == VK_FORMAT_UNDEFINED)
	{
		return ((VkSurfaceFormatKHR){VK_FORMAT_B8G8R8A8_UNORM
				, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
	}
	i = 0;
	while (i < n)
	{
		if (format[i].format == VK_FORMAT_B8G8R8A8_UNORM
			&& format[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format[i];
		i++;
	}
	return format[0];
}

VkPresentModeKHR			swap_choose_present_mode(
	const VkPresentModeKHR *format)
{
	return (VK_PRESENT_MODE_FIFO_KHR);
}

VkExtent2D	swap_choose_extent(const VkSurfaceCapabilitiesKHR capability)
{
	VkExtent2D	actual_ext;

	if (capability.currentExtent.width != UINT32_MAX)
		return (capability.currentExtent);
	actual_ext = (VkExtent2D){800, 600};
	if (capability.maxImageExtent.width < actual_ext.width)
		actual_ext.width = capability.maxImageExtent.width;
	if (capability.minImageExtent.width > actual_ext.width)
		actual_ext.width = capability.minImageExtent.width;
	if (capability.maxImageExtent.height < actual_ext.height)
		actual_ext.height = capability.maxImageExtent.height;
	if (capability.minImageExtent.height > actual_ext.height)
		actual_ext.height = capability.minImageExtent.height;
	return (actual_ext);
}
