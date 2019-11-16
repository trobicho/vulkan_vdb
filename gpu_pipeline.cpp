/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gpu_pipeline.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/22 17:58:08 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/16 02:27:25 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "shader.h"
#include "gpu_pipeline.h"
#include <stdlib.h>
#include "Mesh.h"

#include <stdio.h>

VkPipelineShaderStageCreateInfo	*pipe_shader_create(
	VkShaderModule vert_mod, VkShaderModule frag_mod)
{
	VkPipelineShaderStageCreateInfo			vert_info;
	VkPipelineShaderStageCreateInfo			frag_info;
	VkPipelineShaderStageCreateInfo			*ret_info;

	vert_info = (VkPipelineShaderStageCreateInfo){};
	frag_info = (VkPipelineShaderStageCreateInfo){};

	vert_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_info.module = vert_mod;
	vert_info.pName = "main";

	frag_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_info.module = frag_mod;
	frag_info.pName = "main";
	if ((ret_info = (VkPipelineShaderStageCreateInfo*)
		malloc(sizeof(*ret_info) * 2)) == NULL)
	{
		return (NULL);
	}
	ret_info[0] = vert_info;
	ret_info[1] = frag_info;

	return (ret_info);
}

VkPipelineMultisampleStateCreateInfo	multisampling_create(void)
{
	VkPipelineMultisampleStateCreateInfo multisampling_info;

	multisampling_info = (VkPipelineMultisampleStateCreateInfo){};
	multisampling_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.sampleShadingEnable = VK_FALSE;
	multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling_info.minSampleShading = 1.0f;
	multisampling_info.pSampleMask = NULL;
	multisampling_info.alphaToCoverageEnable = VK_FALSE;
	multisampling_info.alphaToOneEnable = VK_FALSE;
	return (multisampling_info);
}

VkPipelineRasterizationStateCreateInfo	rasterizer_create(void)
{
	VkPipelineRasterizationStateCreateInfo	rasterizer_info;

	rasterizer_info = (VkPipelineRasterizationStateCreateInfo){};
	rasterizer_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = VK_FALSE;
	rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
	rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer_info.lineWidth = 1.0f;
	//rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer_info.cullMode = VK_CULL_MODE_NONE;
	rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer_info.depthBiasEnable = VK_FALSE;
	rasterizer_info.depthBiasConstantFactor = 0.0f;
	rasterizer_info.depthBiasClamp = 0.0f;
	rasterizer_info.depthBiasSlopeFactor = 0.0f;
	return (rasterizer_info);
}

VkPipelineViewportStateCreateInfo		viewport_create(VkExtent2D extent)
{
	VkPipelineViewportStateCreateInfo		viewport_info;
	static VkViewport								viewport;
	static VkRect2D								scissor;

	viewport_info = (VkPipelineViewportStateCreateInfo){};
	viewport = (VkViewport){};
	scissor = (VkRect2D){};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = extent.width;
	viewport.height = extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	scissor.offset = (VkOffset2D){0, 0};
	scissor.extent = extent;
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount = 1;
	viewport_info.pViewports = &viewport;
	viewport_info.scissorCount = 1;
	viewport_info.pScissors = &scissor;
	return (viewport_info);
}

VkPipelineColorBlendStateCreateInfo		color_blend_create(void)
{
	static VkPipelineColorBlendAttachmentState		color_blend;
	VkPipelineColorBlendStateCreateInfo		color_blend_info;

	color_blend = (VkPipelineColorBlendAttachmentState){};
	color_blend_info = (VkPipelineColorBlendStateCreateInfo){};
	color_blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
		| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	color_blend.blendEnable = VK_FALSE;
	color_blend_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_info.logicOpEnable = VK_FALSE;
	color_blend_info.attachmentCount = 1;
	color_blend_info.pAttachments = &color_blend;
	return (color_blend_info);
}
	
VkPipelineInputAssemblyStateCreateInfo	input_assembly_create(void)
{
	VkPipelineInputAssemblyStateCreateInfo	input_assembly;

	input_assembly = (VkPipelineInputAssemblyStateCreateInfo){};
	input_assembly.sType
		= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;
	return (input_assembly);
}

VkPipelineVertexInputStateCreateInfo	vert_input_create(void)
{
	VkPipelineVertexInputStateCreateInfo	vert_input_info;
	static auto binding_description = s_vertex::get_binding_description();
	static auto attribute_descriptions = s_vertex::get_attribute_descriptions();

	vert_input_info = (VkPipelineVertexInputStateCreateInfo){};
	vert_input_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vert_input_info.vertexBindingDescriptionCount = 1;
	vert_input_info.pVertexBindingDescriptions = &binding_description;
	vert_input_info.vertexAttributeDescriptionCount =
		static_cast<uint32_t>(attribute_descriptions.size());
	vert_input_info.pVertexAttributeDescriptions =
		attribute_descriptions.data();
	return (vert_input_info);
}

VkPipelineDepthStencilStateCreateInfo	depth_stencil_create()
{
	VkPipelineDepthStencilStateCreateInfo	depth_stencil_info = {};
	depth_stencil_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_info.depthTestEnable = VK_TRUE;
	depth_stencil_info.depthWriteEnable = VK_TRUE;
	depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_info.minDepthBounds = 0.0f;
	depth_stencil_info.maxDepthBounds = 1.0f;
	depth_stencil_info.stencilTestEnable = VK_FALSE;
	depth_stencil_info.front = {};
	depth_stencil_info.back = {};
	return (depth_stencil_info);
}
