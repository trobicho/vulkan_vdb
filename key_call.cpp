/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_call.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/12 07:23:05 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/15 21:41:00 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "key_call.h"
#include <cmath>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

void	key_call(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	s_user *user;

	user = (s_user*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_ESCAPE)
		user->quit = true;
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
		user->cam_pos += user->cam_dir * 2.0f;
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
		user->cam_pos += user->cam_dir * -2.0f;
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
		user->cam_pos += user->cam_right * -2.0f;
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
		user->cam_pos += user->cam_right * 2.0f;
	
	/*
	if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
		ubo->view = glm::rotate(ubo->view, glm::radians(0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
	if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
		ubo->view = glm::rotate(ubo->view, glm::radians(-0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cam_rotate(ubo, 0.1, vulk->ubo.cam.up);
	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cam_rotate(ubo, -0.1, vulk->ubo.cam.up);
	if (key == GLFW_KEY_KP_7 && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cam_rotate(ubo, -0.1, vulk->ubo.cam.dir);
	if (key == GLFW_KEY_KP_9 && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cam_rotate(ubo, 0.1, vulk->ubo.cam.dir);
	*/
	//std::cout << user->cam_pos.x << ", " << user->cam_pos.y << ", " << user->cam_pos.z << std::endl;
	//std::cout << user->cam_dir.x << ", " << user->cam_dir.y << ", " << user->cam_dir.z << std::endl << std::endl;
}

void	cursor_call(GLFWwindow* window, double x_pos, double y_pos)
{
	s_user			*user;
	static double	last_x = x_pos;
	static double	last_y = y_pos;
	float			norme = sqrt((x_pos - last_x) * (x_pos - last_x)
							+ (y_pos - last_y) * (y_pos - last_y));
	glm::vec3		axis; 

	user = (s_user*)glfwGetWindowUserPointer(window);
	if (norme > 0.01)
	{
		axis = user->cam_right * (float)(y_pos - last_y) + user->cam_up * (float)(last_x - x_pos);
		axis = glm::normalize(axis);
		user->cam_dir = glm::rotate(user->cam_dir, norme * glm::radians(0.1f), axis);
		user->cam_right = glm::rotate(user->cam_right, (float)(last_x - x_pos) * glm::radians(0.1f), user->cam_up);
		last_x = x_pos;
		last_y = y_pos;
	}
}
