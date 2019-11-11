/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 06:56:37 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/11 07:13:35 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include <array>

struct	s_vertex
{
	glm::vec2	pos;
	glm::vec3	color;

	static VkVertexInputBindingDescription	get_binding_description()
	{
		static VkVertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = sizeof(s_vertex);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return (binding_description);
	}
	static std::array<VkVertexInputAttributeDescription, 2>
		get_attribute_descriptions() 
	{
		std::array<VkVertexInputAttributeDescription, 2>
			attribute_descriptions = {};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(s_vertex, pos);
		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(s_vertex, color);
		return (attribute_descriptions);
	}
};

class	Mesh
{
	public:
		Mesh();
		~Mesh(){};

		std::vector<s_vertex>	vertex_buffer;
		std::vector<uint32_t>	index_buffer;
};
