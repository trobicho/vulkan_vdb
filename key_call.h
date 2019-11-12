/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_call.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/12 07:22:15 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/12 08:22:26 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "My_vulkan.h"

struct	s_user
{
	glm::vec3	cam_pos;
	glm::vec3	cam_up;
	glm::vec3	cam_dir;
	s_ubo	ubo;
	bool	quit = false;
};

void	key_call(GLFWwindow* window, int key, int scancode, int action, int mods);
void	cursor_call(GLFWwindow* window, double x_pos, double y_pos);
