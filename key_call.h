/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_call.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/12 07:22:15 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/17 17:57:23 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Player.h"

struct	s_user
{
	s_user(Player &p): player(p) {};
	Player	&player;
	bool	quit = false;
};

void	key_call(GLFWwindow* window, int key, int scancode, int action, int mods);
void	cursor_call(GLFWwindow* window, double x_pos, double y_pos);
