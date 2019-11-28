/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 20:39:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/28 21:41:19 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <thread>
#include <iostream>
#include "Map_loader.h"
#include "key_call.h"
#include "Physic.h"

static int	main_loop(My_vulkan &my_vulkan, Map_loader &map_loader
				, GLFWwindow *win)
{
	s_user	*user = (s_user*)glfwGetWindowUserPointer(win);
	Physic	physic(map_loader.get_vdb_ref());

	while(!glfwWindowShouldClose(win) && !user->quit)
	{
		glfwPollEvents();
		if (user->resync_physic_time)
		{
			physic.resync_time();
			user->resync_physic_time = false;
		}
		user->player.move();
		if (user->player.has_physic())
			physic.apply_physic_to_player(user->player);
		user->player.update_ubo();
		if (map_loader.has_update())
		{
			map_loader.update();
			if (my_vulkan.copy_vertex_buffer() == -1)
				return (-1);
			if (my_vulkan.copy_vertex_index_buffer() == -1)
				return (-1);
			if (my_vulkan.command_buffer_record() == -1)
				return (-1);
		}
		my_vulkan.draw_frame();
	}
	map_loader.quit();
	vkDeviceWaitIdle(my_vulkan.get_device_ref());
	return (0);
}

int	main()
{
	Vdb_test	my_vdb;
	int			xr = trl::rand_uniform_int(500, 8192 - 500);
	int			zr = trl::rand_uniform_int(500, 8192 - 500);

	xr = 4096;
	zr = 4096;

	Player		player(glm::vec3((float)xr, 130.0f, (float)zr));
	Map_loader	map_loader(my_vdb, player);
	map_loader.load_one_chunck(xr, zr);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *win = glfwCreateWindow(1920, 1080, "Vulkan"
		, glfwGetPrimaryMonitor(), NULL);
	//GLFWwindow *win = glfwCreateWindow(800, 600, "Vulkan"
		//, NULL, NULL);


	s_user		user(player);
	player.get_cam_ref().ubo.sun_pos = glm::vec3(xr, 300, zr);

	My_vulkan	my_vulkan(win, map_loader.get_mesh_ref()
					, player.get_cam_ref().ubo);
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

	std::thread thread(&Map_loader::thread_loader, &map_loader);

	main_loop(my_vulkan, map_loader, win);
	thread.join();
	glm::vec3 v = player.get_pos();
	std::cout << "m_pos = {" << v.x << ", "
				<< v.y << ", " << v.z << "}" << std::endl;
	return (0);
}
