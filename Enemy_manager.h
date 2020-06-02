/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Enemy_manager.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/21 19:59:25 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/27 16:05:51 by trobicho         ###   ########.fr       */
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

		int			update(const Vdb_test &world);
		int			init();
		inline std::vector<glm::mat4>
					&get_bones_ref(){return (m_spider.get_bones_ref());}
		inline Spider
					&get_spider_ref(){return (m_spider);}

	private:
		My_vulkan	&m_vulk;
		Spider		m_spider;
		s_enemy		m_spider_vbo;
};
