/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_call.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/12 07:23:05 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/19 04:21:55 by trobicho         ###   ########.fr       */
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
	if (key == GLFW_KEY_F)
	{
		if (action == GLFW_PRESS)
		{
			user->player.sync_to_cam();
			user->player.alternate_mode();
			user->resync_physic_time = true;
		}
	}
	if (key == GLFW_KEY_SPACE)
	{
		if (action == GLFW_PRESS)
		{
			if (!user->player.is_falling())
				user->player.jump();
		}
	}
	if (key == GLFW_KEY_B)
	{
		if (action == GLFW_PRESS)
		{
			if (!user->player.is_falling())
				user->player.super_jump();
		}
	}
	if (key == GLFW_KEY_W)
	{
		if (action == GLFW_PRESS)
			user->player.set_state(P_STATE_FORWARD);
		else if (action == GLFW_RELEASE)
			user->player.unset_state(P_STATE_FORWARD);
	}
	if (key == GLFW_KEY_S)
	{
		if (action == GLFW_PRESS)
			user->player.set_state(P_STATE_BACKWARD);
		else if (action == GLFW_RELEASE)
			user->player.unset_state(P_STATE_BACKWARD);
	}
	if (key == GLFW_KEY_D)
	{
		if (action == GLFW_PRESS)
			user->player.set_state(P_STATE_RIGHT);
		else if (action == GLFW_RELEASE)
			user->player.unset_state(P_STATE_RIGHT);
	}
	if (key == GLFW_KEY_A)
	{
		if (action == GLFW_PRESS)
			user->player.set_state(P_STATE_LEFT);
		else if (action == GLFW_RELEASE)
			user->player.unset_state(P_STATE_LEFT);
	}
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
	Camera	&cam = user->player.get_cam_ref();
	if (norme > 0.01)
	{
		axis = cam.right * (float)(y_pos - last_y) + cam.up * (float)(last_x - x_pos);
		axis = glm::normalize(axis);
		cam.dir = glm::rotate(cam.dir, norme * glm::radians(0.1f), axis);
		cam.right = glm::rotate(cam.right, (float)(last_x - x_pos) * glm::radians(0.1f), cam.up);
		last_x = x_pos;
		last_y = y_pos;
	}
}
