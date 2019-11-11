/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gpu_pipeline.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/22 17:58:06 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/09 22:40:04 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

VkPipelineShaderStageCreateInfo	*pipe_shader_create(
	VkShaderModule vert_mod, VkShaderModule frag_mod);
VkPipelineMultisampleStateCreateInfo	multisampling_create(void);
VkPipelineRasterizationStateCreateInfo	rasterizer_create(void);
VkPipelineViewportStateCreateInfo		viewport_create(VkExtent2D extent);
VkPipelineColorBlendStateCreateInfo		color_blend_create(void);
VkPipelineInputAssemblyStateCreateInfo	input_assembly_create(void);
VkPipelineVertexInputStateCreateInfo	vert_input_create(void);
