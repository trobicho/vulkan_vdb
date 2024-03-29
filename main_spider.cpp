/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_spider.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 20:39:09 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/21 11:20:50 by trobicho         ###   ########.fr       */
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

	while(!glfwWindowShouldClose(win) && !user->quit)
	{
		glfwPollEvents();
		user->player.move();
		auto &cam = user->player.get_cam_ref();
		auto &bones = enemy_manager.get_bones_ref();
		for (int i = 0; i < 17; ++i)
		{
			cam.ubo.bone[i] = bones[i];
		}
		enemy_manager.update();
		user->player.update_ubo();
		my_vulkan.draw_frame();
	}
	vkDeviceWaitIdle(my_vulkan.get_device_ref());
	return (0);
}

#include "Noise.h"

int	main()
{
	Internal_enemy	root_node = Internal_enemy(0, 0, 0);
	Vdb_test		my_vdb(root_node);
	int				xr = 0;
	int				zr = 0;
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *win = glfwCreateWindow(1920, 1080, "Vulkan"
		, glfwGetPrimaryMonitor(), NULL);
	
	Player		player(glm::vec3((float)xr, 0.0f, (float)zr));

	My_vulkan	my_vulkan(win, player.get_cam_ref().ubo);
	Map_loader	map_loader(my_vdb, my_vulkan, player);


	glm::vec3 v = player.get_cam_pos();
	std::cout << "m_pos = {" << v.x << ", "
				<< v.y << ", " << v.z << "}" << std::endl;

	s_vbox box;
	box.origin = s_vec3i(xr, 0, zr);
	box.len = s_vec3i(1 << CHUNK_LOG_X, 1 << CHUNK_LOG_Y, 1 << CHUNK_LOG_Z);
	map_loader.load_pos(box.origin);

	s_user		user(player, my_vdb, map_loader);

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
		return (1);
	}

	Enemy_manager	enemy_manager = Enemy_manager(my_vulkan);
	if (enemy_manager.init() == -1)
	{
		std::cout << "Unable to initialize Enemy command buffer !" << std::endl;
		return (1);
	}

	main_loop(my_vulkan, map_loader, win, enemy_manager);
	v = player.get_cam_pos();
	std::cout << "m_pos = {" << v.x << ", "
				<< v.y << ", " << v.z << "}" << std::endl;
	return (0);
}
