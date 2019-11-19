/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 06:56:37 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/19 20:57:06 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <array>

struct	s_vertex
{
	s_vertex():color(glm::vec3(0.1f, 0.7f, 0.15f)){};
	s_vertex(glm::vec3 p_pos, glm::vec3 p_color = glm::vec3(0.7f, 0.7f, 0.7f)):
		pos(p_pos), color(p_color){};

	glm::vec3	pos;
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
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
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

		uint32_t	get_nb_vertex() const {return(vertex_buffer.size());}
		uint32_t	get_nb_index() const {return(index_buffer.size());}
		int			add_vertex(s_vertex v, uint32_t idx);
		int			add_vertex_with_basic_index(s_vertex v);
		void		add_index(uint32_t idx);
		void		reset();

		std::vector<s_vertex>	vertex_buffer;
		std::vector<uint32_t>	index_buffer;
};
