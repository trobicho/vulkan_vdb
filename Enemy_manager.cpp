/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Enemy_manager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/21 20:04:13 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/23 22:51:08 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Enemy_manager.h"

Enemy_manager::Enemy_manager(My_vulkan &vulk): m_vulk(vulk)
{
	m_spider.generate();
}

int		Enemy_manager::update()
{
	m_spider_vbo.nb_index = m_spider.get_mesh_ref().index_buffer.size();
	std::cout << "nb index: " << m_spider_vbo.nb_index << std::endl;
	m_spider_vbo.update(m_vulk, m_spider.get_mesh_ref());
	if (m_vulk.command_buffer_record(m_spider_vbo) == -1)
		return (-1);
	return (0);
}
