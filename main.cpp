/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 20:39:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/26 00:16:49 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <SDL2/SDL_image.h>
#include "Map.h"
#include "My_vulkan.h"
#include "key_call.h"
#include "Physic.h"
#include "Moore_accessor.h"

/*
double	get_noise_value_from_img(SDL_Surface *surface, double x, double y)
{
	double	v;
	int ix = x * surface->w;
	int iy = y * surface->h;

	v = ((Uint8*)surface->pixels)[ix + iy * surface->w] / 255.0;
	return (v);
}

int		add_voxel_from_img(Vdb_test &vdb, const char *file_name, s_vbox box)
{
	SDL_Surface *img = IMG_Load(file_name);
	if (!img)
	{
		std::cout << SDL_GetError() << std::endl;
		return (1);
	}
	std::cout << "Bpp: " << img->format->BitsPerPixel << std::endl;
	for (int z = 0; z < box.len.z; ++z)
	{
		for (int x = 0; x < box.len.x; ++x)
		{
			double	d = get_noise_value_from_img(img, (double)x / box.len.x
				, (double)z / box.len.z);
			for (int y = 0; y < box.len.y * d + 1; ++y)
			{
				s_vec3i	vox(x, y, z);
				vox.x += box.origin.x;
				vox.y += box.origin.y;
				vox.z += box.origin.z;
				vdb.set_vox(1, vox);
				nb_vox++;
				if (nb_vox > 0 && nb_vox % 200000000 == 0)
				{
					vdb.pruning();
					std::cout << "prunning" << std::endl;
					std::cout << (z / (double)box.len.x) * 100.0 << "%"  << std::endl;
					std::cout << "total of " << nb_vox << " voxels." << std::endl;
				}
			}
		}
	}
	SDL_FreeSurface(img);
	return (0);
}
*/

static int	main_loop(My_vulkan &my_vulkan, Vdb_test &vdb
	, Mesh &mesh, GLFWwindow *win)
{
	s_user	*user = (s_user*)glfwGetWindowUserPointer(win);
	Physic	physic(vdb);

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
		/*
		if (mesh.has_update())
		{
			if (my_vulkan.command_buffer_record() == -1)
				return (-1);
		}
		*/
		my_vulkan.draw_frame();
	}
	vkDeviceWaitIdle(my_vulkan.get_device_ref());
	return (0);
}

int	main()
{
	Vdb_test	my_vdb;
	auto&		mt = trl::req_mt_ref();
	std::uniform_int_distribution<int>
			dis(0, 1000);
	s_vbox		box;
	Moore_accessor
				moore_access(my_vdb);
	Mesh		mesh(moore_access);
	Map			map(0);
	int			xr = trl::rand_uniform_int(500, 8192 - 500);
	int			zr = trl::rand_uniform_int(500, 8192 - 500);

	xr = 4096;
	zr = 4096;
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *win = glfwCreateWindow(1920, 1080, "Vulkan"
		, glfwGetPrimaryMonitor(), NULL);
	//GLFWwindow *win = glfwCreateWindow(800, 600, "Vulkan"
		//, NULL, NULL);


	mesh.reset();
	box.len = s_vec3i(12, 128, 12);
	for (int x = 0; x < 1; x++)
	{
		for (int z = 0; z < 1; z++)
		{
			box.origin.x = xr + x * box.len.x;
			box.origin.z = zr + z * box.len.z;
			box.origin.y = 0;
			if (map.generate(my_vdb, box))
				return (1);
			std::cout << x << "/4, " << z << "/4" << std::endl;
		}
	}
	my_vdb.pruning();
	my_vdb.mesh(mesh, box);
	mesh.remove_vertex(100000, 30000);
	//box.origin = s_vec3i(xr - box.len.x / 2, 0, zr - box.len.z / 2);
	std::cout << "total of " << map.get_nb_vox() << " voxels." << std::endl;
	std::cout << "total of " << mesh.get_nb_vertex() << " vertex." << std::endl;
	std::cout << "total of " << mesh.get_nb_index() << " index." << std::endl;
	std::cout << std::endl;
	
	Player		player(glm::vec3((float)xr, 130.0f, (float)zr));
	glm::vec3 v = player.get_pos();
	std::cout << "m_pos = {" << v.x << ", "
				<< v.y << ", " << v.z << "}" << std::endl;
	s_user		user(player);
	player.get_cam_ref().ubo.sun_pos = glm::vec3(xr, 300, zr);
	My_vulkan	my_vulkan(win, mesh, player.get_cam_ref().ubo);
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

	main_loop(my_vulkan, my_vdb, mesh, win);
	v = player.get_pos();
	std::cout << "m_pos = {" << v.x << ", "
				<< v.y << ", " << v.z << "}" << std::endl;
	return (0);
}
