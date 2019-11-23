/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 06:56:37 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/23 05:31:21 by trobicho         ###   ########.fr       */
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
#include <bitset>
#include "Block.h"

struct	s_vec3i
{
	s_vec3i(){};
	s_vec3i(int32_t sx, int32_t sy, int32_t sz): x(sx), y(sy), z(sz){};
	int32_t	x, y, z;
};

class	Moore_accessor;

struct	s_vertex
{
	s_vertex():color(glm::vec3(0.1f, 0.7f, 0.15f)){};
	s_vertex(glm::vec3 p_pos, glm::vec3 p_color = glm::vec3(0.7f, 0.7f, 0.7f)):
		pos(p_pos), color(p_color){};

	glm::vec3				pos;
	glm::vec3				color;
	uint8_t					ao = 3;

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
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(s_vertex, color);
		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R8_UINT;
		attribute_descriptions[2].offset = offsetof(s_vertex, ao);
		return (attribute_descriptions);
	}
};

class	Mesh
{
	public:
		Mesh(Moore_accessor &moore_access);
		~Mesh(){};

		void		reset();
		uint32_t	get_nb_vertex() const {return(vertex_buffer.size());}
		uint32_t	get_nb_index() const {return(index_buffer.size());}
		int			add_vertex(s_vertex v, uint32_t idx);
		int			add_vertex_with_basic_index(s_vertex v);
		int			add_vertex_with_no_index(s_vertex v, uint8_t ao = 3);
		void		add_index(uint32_t idx);
		void		add_cube_from_node(s_vec3i v
						, e_block_type type, void *node_ptr);

		std::vector<s_vertex>	vertex_buffer;
		std::vector<uint32_t>	index_buffer;

	private:
		void		get_needed_vertex(std::bitset<8> &v_b);
		void		add_needed_vertex(s_vertex v, s_vec3i l
						, std::bitset<8> &v_b, uint32_t v_idx[8]);
		uint8_t		get_ao(int idx);
		uint8_t		vertex_ao(bool side1, bool side2, bool corner)
		{
			if(side1 && side2)
				return (0);
			return (3 - (side1 + side2 + corner));
		}

		Moore_accessor		&m_moore_access;
};
