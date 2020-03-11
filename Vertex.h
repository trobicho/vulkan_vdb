/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vertex.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/24 19:25:39 by trobicho          #+#    #+#             */
/*   Updated: 2020/01/21 11:17:55 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <array>
#include "Block.h"

struct	s_vertex
{
	s_vertex():color(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)){};
	s_vertex(glm::vec3 p_pos, uint32_t block): pos(p_pos)
	{
		set_prop_from_block(block);
	}
	s_vertex(glm::vec3 p_pos
		, glm::vec4 p_color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f))
		: pos(p_pos), color(p_color){};

	glm::vec3				pos;
	glm::vec4				color;
	uint8_t					ao = 3;

	void	set_prop_from_block(uint32_t block)
	{
		color = get_color_from_block_type((e_block_type)block);
	}

	static VkVertexInputBindingDescription	get_binding_description()
	{
		static VkVertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = sizeof(s_vertex);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return (binding_description);
	}
	static std::array<VkVertexInputAttributeDescription, 3>
		get_attribute_descriptions() 
	{
		std::array<VkVertexInputAttributeDescription, 3>
			attribute_descriptions = {};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(s_vertex, pos);
		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(s_vertex, color);
		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R8_UINT;
		attribute_descriptions[2].offset = offsetof(s_vertex, ao);
		return (attribute_descriptions);
	}
};

struct	s_vertex_bones
{
	glm::vec3	pos;
	glm::vec3	color = {0.1f, 0.1f , 0.1f};
	int			index = 0;
	float		weight = 1.0f;
	uint8_t		ao;

	s_vertex_bones():color(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)){};
	s_vertex_bones(glm::vec3 p_pos, uint32_t block): pos(p_pos)
	{
		set_prop_from_block(block);
	}
	s_vertex_bones(glm::vec3 p_pos
		, glm::vec4 p_color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f))
		: pos(p_pos), color(p_color){};
	
	void	set_prop_from_block(uint32_t block)
	{
		color = glm::vec4(0.7, 0.7, 0.7, 1.0f);
		index = block;
	}

	static VkVertexInputBindingDescription	get_binding_description()
	{
		static VkVertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = sizeof(s_vertex_bones);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return (binding_description);
	}
	static std::array<VkVertexInputAttributeDescription, 5>
		get_attribute_descriptions() 
	{
		std::array<VkVertexInputAttributeDescription, 5>
			attribute_descriptions = {};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(s_vertex_bones, pos);

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(s_vertex_bones, color);

		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R32_UINT;
		attribute_descriptions[2].offset = offsetof(s_vertex_bones, index);
		
		attribute_descriptions[3].binding = 0;
		attribute_descriptions[3].location = 3;
		attribute_descriptions[3].format = VK_FORMAT_R32_SFLOAT;
		attribute_descriptions[3].offset = offsetof(s_vertex_bones, weight);
		
		attribute_descriptions[4].binding = 0;
		attribute_descriptions[4].location = 4;
		attribute_descriptions[4].format = VK_FORMAT_R8_UINT;
		attribute_descriptions[4].offset = offsetof(s_vertex_bones, ao);
		return (attribute_descriptions);
	}
};
