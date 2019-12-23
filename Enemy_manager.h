/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Enemy_manager.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/21 19:59:25 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/21 21:14:35 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "My_vulkan.h"
#include "Spider.h"

class Enemy_manager
{
	public:
		Enemy_manager(My_vulkan &vulk);
		~Enemy_manager(){};

		int		update();

	private:
		My_vulkan	&m_vulk;
		Spider		m_spider;
		s_enemy		m_spider_vbo;
};
