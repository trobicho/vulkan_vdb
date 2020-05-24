/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gpu_pipeline.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/22 17:58:06 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/22 13:14:10 by trobicho         ###   ########.fr       */
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
VkPipelineVertexInputStateCreateInfo	vert_bones_input_create(void);
VkPipelineDepthStencilStateCreateInfo	depth_stencil_create();
