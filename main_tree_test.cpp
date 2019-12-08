/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_tree_test.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/04 17:13:03 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/08 01:31:33 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include "Mesh.h"
#include "key_call.h"
#include "Physic.h"
#include "Vdb_test.h"
#include "Moore_accessor.h"
#include "Lsystem.h"
#include "Space_colonisation.h"
#include "my_lib.h"

bool	m_update = false;

static void	generate_attractor(s_space_col_info &sc_inf, int n
				, glm::vec3 center, double r)
{
	int			nb_att = 0;
	glm::vec3	v;

	while (nb_att < n)
	{
		v.x = trl::rand_uniform_int(-r, r);
		v.y = trl::rand_uniform_int(-r, r);
		v.z = trl::rand_uniform_int(-r, r);
		if (length(v) > r)
			continue;
		v += center;
		sc_inf.attractor.push_back(v);
		nb_att++;
	}
}

static int		update(My_vulkan &vulk, Mesh &mesh)
{
	static size_t	m_old_size_vbo = 0;
	static size_t	m_old_size_ibo = 0;
	VkBuffer		staging_buffer;
	VkBuffer		staging_buffer_idx;
	VkDeviceMemory	staging_buffer_memory;
	VkDeviceMemory	staging_buffer_memory_idx;
	VkDeviceSize	copy_size;
	VkDeviceSize	copy_size_idx;
	void*			data;
	void*			data_idx;
	const VkDevice	&device_ref = vulk.get_device_ref();

	copy_size = sizeof(mesh.vertex_buffer[0])
				* (mesh.vertex_buffer.size() - m_old_size_vbo);
	copy_size_idx = sizeof(mesh.index_buffer[0])
				* (mesh.index_buffer.size() - m_old_size_ibo);
	if (copy_size == 0 || copy_size_idx == 0)
	{
		m_update = false;
		return (0);
	}
	if (vulk.create_buffer(copy_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, staging_buffer, staging_buffer_memory) == -1)
	{
		return (-1);
	}
	if (vulk.create_buffer(copy_size_idx, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, staging_buffer_idx, staging_buffer_memory_idx) == -1)
	{
		return (-1);
	}
	vkMapMemory(device_ref, staging_buffer_memory, 0, copy_size, 0, &data);
	vkMapMemory(device_ref, staging_buffer_memory_idx, 0
			, copy_size_idx, 0, &data_idx);
	memcpy(((char*)data), mesh.vertex_buffer.data() + m_old_size_vbo
			, copy_size);
	memcpy(((char*)data_idx), mesh.index_buffer.data() + m_old_size_ibo
			, copy_size_idx);
	vkUnmapMemory(device_ref, staging_buffer_memory);
	vkUnmapMemory(device_ref, staging_buffer_memory_idx);
	vulk.copy_staging_to_vbo(staging_buffer, staging_buffer_memory
			, copy_size, m_old_size_vbo * sizeof(mesh.vertex_buffer[0]));
	vulk.copy_staging_to_ibo(staging_buffer_idx, staging_buffer_memory_idx
			, copy_size_idx, m_old_size_ibo * sizeof(mesh.index_buffer[0]));
	if (vulk.command_buffer_record(mesh.index_buffer.size()) == -1)
		return (-1);
	m_old_size_vbo = mesh.vertex_buffer.size();
	m_old_size_ibo = mesh.index_buffer.size();
	if (vulk.command_buffer_record(mesh.index_buffer.size()) == -1)
		return (-1);
	m_update = false;
	return (0);
}


static int	main_loop(My_vulkan &my_vulkan, Mesh &mesh, GLFWwindow *win)
{
	s_user	*user = (s_user*)glfwGetWindowUserPointer(win);

	while(!glfwWindowShouldClose(win) && !user->quit)
	{
		glfwPollEvents();
		user->player.move();
		user->player.update_ubo();
		/*
		if (map_loader.has_update())
		{
			map_loader.update();
			if (my_vulkan.copy_vertex_buffer() == -1)
				return (-1);
			if (my_vulkan.copy_vertex_index_buffer() == -1)
				return (-1);
		}
		*/
		my_vulkan.draw_frame();
	}
	vkDeviceWaitIdle(my_vulkan.get_device_ref());
	return (0);
}

#include "Noise.h"

int		main()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *win = glfwCreateWindow(1920, 1080, "Vulkan"
		, glfwGetPrimaryMonitor(), NULL);
	
	Player		player(glm::vec3(80, 0, 80));
	
	My_vulkan	my_vulkan(win, player.get_cam_ref().ubo);

	Vdb_test		vdb;
	s_user		user(player, vdb);

	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetWindowUserPointer(win, &user);
	glfwSetKeyCallback(win, key_call);
	glfwSetCursorPosCallback(win, cursor_call);

	if (my_vulkan.init())
	{
		std::cout << "Unable to initialize Vulkan !" << std::endl;
	}

	Moore_accessor	m_a = Moore_accessor(vdb);
	Mesh			mesh(m_a);
	Lsystem			lsystem(mesh);
	
	s_space_col_info	info;

	info.attractor.reserve(2000);
	info.branch.reserve(2000);

	info.branch.push_back(Branch(glm::vec3(100, 0, 100)));
	info.di = 8.0;
	info.dk = 2.5;

	generate_attractor(info, 1000, glm::vec3(100.0, 25.0, 100.0), 20);
	Space_colonisation	space_col(info, vdb);
	space_col.grow_full(400);
	vdb.mesh(mesh);
	m_update = true;
	update(my_vulkan, mesh);
	main_loop(my_vulkan, mesh, win);
	return (0);
}
