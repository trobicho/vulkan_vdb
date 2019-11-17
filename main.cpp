/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/02 20:39:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/17 07:46:26 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Vdb_test.h"
#include "my_lib.h"
#include <SDL2/SDL_image.h>
#include "My_vulkan.h"
#include "key_call.h"
#include "perlin.h"

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

int		add_voxel_from_perlin(Vdb_test &vdb, s_vbox box)
{
	Perlin_noiser	noise;
	double			scalar = 150.0;
	double			scalar_cave = 30.0;
	double			cave_thres = 0.3;
	double			cave_thres_d;
	int				lerp_mod = 3;
	double			d_cave_prec, d_cave_next, d_cave;

	for (int z = 0; z < box.len.z; ++z)
	{
		for (int x = 0; x < box.len.x; ++x)
		{
			double	d = noise.perlin2d(3, 2., 0.5
						, (double)(x + box.origin.x) / scalar
						, (double)(z + box.origin.z) / scalar);
			for (int y = 0; y < box.len.y * d + 1; ++y)
			{
				s_vec3i	vox(x, y, z);
				if (y % lerp_mod == 0)
				{
					d_cave = noise.perlin3d(3, 1.5, 0.5
						, (double)(x + box.origin.x) / scalar_cave
						, (double)(z + box.origin.z) / scalar_cave
						, (double)(y + box.origin.y) / scalar_cave);
					d_cave_prec = d_cave;
					d_cave_next = noise.perlin3d(3, 1.5, 0.5
						, (double)(x + box.origin.x) / scalar_cave
						, (double)(z + box.origin.z) / scalar_cave
						, (double)(y + box.origin.y + lerp_mod) / scalar_cave);
				}
				else
				{
					d_cave = Perlin_noiser::lerp(d_cave_prec, d_cave_next
								, (y % lerp_mod) / (double)lerp_mod);
				}
				cave_thres_d = (1.0 - (double)y / box.len.y) - cave_thres;
				if (d_cave < cave_thres)
					continue ;
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
	return (0);
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
	int			xr = trl::rand_uniform_int(256, 8192 - 256);
	int			zr = trl::rand_uniform_int(256, 8192 - 256);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *win = glfwCreateWindow(1920, 1080, "Vulkan", glfwGetPrimaryMonitor(), NULL);
	
	IMG_Init(IMG_INIT_PNG);
	box.len = s_vec3i(300, 128, 300);
	box.origin = s_vec3i(xr - box.len.x / 2, 0, zr - box.len.z / 2);
	if (add_voxel_from_perlin(my_vdb, box))
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
	
	user.ubo.model = glm::rotate(glm::mat4(1.0f)
						, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	user.cam_pos = glm::vec3(xr, 130.0f, zr);
	user.cam_dir = glm::vec3(0.0f, 0.0f, 1.0f);
	user.cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
	user.cam_right = glm::vec3(1.0f, 0.0f, 0.0f);

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
