/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 20:39:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/12 14:29:14 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Vdb_test.h"
#include "my_lib.h"
#include <SDL2/SDL_image.h>
#include "My_vulkan.h"
#include "key_call.h"

static uint64_t	nb_vox = 0;

struct	s_vbox
{
	s_vec3i	origin;
	s_vec3i	len;
};

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
				vox.y = (box.origin.y + box.len.y) - y;
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

static void	main_loop(My_vulkan &my_vulkan, GLFWwindow *win)
{
	s_user *user = (s_user*)glfwGetWindowUserPointer(win);
	while(!glfwWindowShouldClose(win) && !user->quit)
	{
		glfwPollEvents();
		user->ubo.view = glm::lookAt(user->cam_pos, user->cam_pos + user->cam_dir, user->cam_up);
		my_vulkan.draw_frame();
	}
	vkDeviceWaitIdle(my_vulkan.get_device_ref());
}

int	main()
{
	Vdb_test	my_vdb;
	auto&		mt = trl::req_mt_ref();
	std::uniform_int_distribution<int>
			dis(0, 1000);
	s_vbox		box;
	Mesh		mesh;
	s_user		user;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *win = glfwCreateWindow(800, 800, "Vulkan", NULL, NULL);
	
	IMG_Init(IMG_INIT_PNG);
	box.origin = s_vec3i(128, 0, 0);
	box.len = s_vec3i(5, 2, 5);
	if (add_voxel_from_img(my_vdb, "./map_img/noise3d.png", box))
		return (1);
	IMG_Quit();
	my_vdb.pruning();
	std::cout << "total of " << nb_vox << " voxels." << std::endl;
	my_vdb.mesh(mesh);
	std::cout << "total of " << mesh.get_nb_vertex() << " vertex." << std::endl;
	std::cout << "total of " << mesh.get_nb_index() << " index." << std::endl;
	/*
	for (int i = 0; i < mesh.get_nb_vertex(); i++)
	{
		std::cout << "vertex[" << i << "]:";
		std::cout << mesh.vertex_buffer[i].pos.x 
			<< ", " << mesh.vertex_buffer[i].pos.y 
			<< ", " << mesh.vertex_buffer[i].pos.z << std::endl;
	}
	*/
	std::cout << std::endl;
	
	user.ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	user.cam_pos = glm::vec3(-1.0f, 15.0f, 0.0f);
	user.cam_dir = glm::vec3(0.0f, 0.0f, 1.0f);
	user.cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

	My_vulkan	my_vulkan(win, mesh, user.ubo);
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

	main_loop(my_vulkan, win);
	return (0);
}
