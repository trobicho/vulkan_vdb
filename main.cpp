/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 20:39:09 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/10 14:01:13 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <thread>
#include <iostream>
#include "Map_loader.h"
#include "key_call.h"
#include "Physic.h"
#include "Enemy_manager.h"

static int	main_loop(My_vulkan &my_vulkan, Map_loader &map_loader
				, GLFWwindow *win, Enemy_manager &enemy_manager)
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
		enemy_manager.get_spider_ref().set_target_world(user->player.get_pos());
		physic.apply_physic(enemy_manager.get_spider_ref());
		if (map_loader.has_update())
		{
			map_loader.update();
			/*
			if (my_vulkan.copy_vertex_buffer() == -1)
				return (-1);
			if (my_vulkan.copy_vertex_index_buffer() == -1)
				return (-1);
			*/
		}
		auto &cam = user->player.get_cam_ref();
		auto &bones = enemy_manager.get_bones_ref();
		for (int i = 0; i < 17; ++i)
		{
			cam.ubo.bone[i] = bones[i];
		}
		cam.ubo.pos_enemy =
				glm::vec4(enemy_manager.get_spider_ref().get_pos(), 1.0f);
		enemy_manager.update();
		user->player.update_ubo();
		my_vulkan.draw_frame();
	}
	map_loader.quit();
	vkDeviceWaitIdle(my_vulkan.get_device_ref());
	return (0);
}

#include "Noise.h"

int	main()
{
	Internal_L1	root_node(0, 0, 0);
	Vdb_test	my_vdb(root_node);
	int			xr = trl::rand_uniform_int(0, 1048575);
	int			zr = trl::rand_uniform_int(0, 1048575);
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *win = glfwCreateWindow(1920, 1080, "Vulkan"
		, glfwGetPrimaryMonitor(), NULL);
	//GLFWwindow *win = glfwCreateWindow(800, 600, "Vulkan"
		//, NULL, NULL);

	/*
	//----------TEST---------
	Noise noise;

	double min = 1.;
	double max = -1.;
	for (int i = 0; i < 100000; i++)
	{
		int			xrand = trl::rand_uniform_int(0, 1048575);
		int			zrand = trl::rand_uniform_int(0, 1048575);
		double		d = noise.smooth_noise2d(xrand / 10., zrand / 10.);
		if (min > d)
			min = d;
		if (max < d)
			max = d;
	}
	std::cout << min << ", " << max << std::endl;
	//----------TEST---------
	*/
	
	xr = 972;
	zr = 1416;
	Player		player(glm::vec3((float)xr, (float)100
					, (float)zr));
	
	My_vulkan	my_vulkan(win, player.get_cam_ref().ubo);
	Map_loader	map_loader(my_vdb, my_vulkan, player);


	glm::vec3 v = player.get_pos();
	std::cout << "m_pos = {" << v.x << ", "
				<< v.y << ", " << v.z << "}" << std::endl;
	s_vbox box;
	box.origin = s_vec3i(0, 0, 0);
	box.len = s_vec3i(1 << CHUNK_LOG_X, 1 << CHUNK_LOG_Y, 1 << CHUNK_LOG_Z);
	map_loader.load_pos(box.origin);

	s_user		user(player, my_vdb, map_loader);
	//player.get_cam_ref().ubo.sun_pos = glm::vec3(xr, 300, zr);

	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetWindowUserPointer(win, &user);
	glfwSetKeyCallback(win, key_call);
	glfwSetCursorPosCallback(win, cursor_call);
	glfwSetCursorPosCallback(win, cursor_call);
	glfwSetMouseButtonCallback(win, mouse_button_call);

	if (my_vulkan.init())
	{
		std::cout << "Unable to initialize Vulkan !" << std::endl;
	}
	Enemy_manager	enemy_manager = Enemy_manager(my_vulkan, my_vdb
		, player.get_pos() + glm::vec3(0.f, 0.f, 20.f));
	if (enemy_manager.init() == -1)
	{
		std::cout << "Unable to initialize Enemy command buffer !" << std::endl;
		return (1);
	}
	map_loader.update();
	std::thread thread(&Map_loader::thread_loader, &map_loader);

	main_loop(my_vulkan, map_loader, win, enemy_manager);
	thread.join();
	v = player.get_pos();
	std::cout << "m_pos = {" << v.x << ", "
				<< v.y << ", " << v.z << "}" << std::endl;
	return (0);
}
