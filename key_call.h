/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_call.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/12 07:22:15 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/08 01:37:55 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Player.h"

struct	s_user
{
	s_user(Player &p, Vdb_test &vdb_ref): player(p), vdb(vdb_ref) {};
	Player		&player;
	Vdb_test	&vdb;
	bool		quit = false;
	bool		resync_physic_time = false;
};

void	key_call(GLFWwindow* window, int key, int scancode, int action, int mods);
void	cursor_call(GLFWwindow* window, double x_pos, double y_pos);
void	mouse_button_call(GLFWwindow* window, int button, int action, int mod);
