/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Enemy_manager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/21 20:04:13 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/24 14:06:30 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Enemy_manager.h"
#include "shader.h"
#include "gpu_pipeline.h"

Enemy_manager::Enemy_manager(My_vulkan &vulk, const Vdb_test &world)
	: m_vulk(vulk), m_world(world), m_spider(world)
{
	m_spider.generate();
}

int		Enemy_manager::init()
{
	m_spider_vbo.nb_index = m_spider.get_mesh_ref().index_buffer.size();
	std::cout << "nb index: " << m_spider_vbo.nb_index << std::endl;
	m_spider_vbo.update(m_vulk, m_spider.get_mesh_ref());
	if (m_vulk.command_enemy_record(m_spider_vbo) == -1)
		return (-1);
	return (0);
}

int		Enemy_manager::update()
{
	m_spider.bones_test();
	return (0);
}
