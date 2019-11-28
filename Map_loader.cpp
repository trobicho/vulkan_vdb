/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:20 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/28 18:43:52 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Map_loader.h"
#include <iostream>

Map_loader::Map_loader(Vdb_test &my_vdb): m_vdb(my_vdb), m_map(0)
					, m_moore_access(m_vdb), m_mesh(m_moore_access)
{
	m_mesh.reset();
}

int		Map_loader::load_one_chunck(uint32_t x, uint32_t z)
{
	s_vbox		box;

	auto	time = std::chrono::high_resolution_clock::now();
	box.len = s_vec3i(64, 128, 64);
	box.origin = s_vec3i(x, 0, z);
	if (m_map.generate(m_vdb, box))
		return (1);
	auto	gtime = std::chrono::high_resolution_clock::now();
	auto	time_gen = std::chrono::duration<float
			, std::chrono::seconds::period>(gtime - time).count();
	m_vdb.pruning();
	std::cout << "time to generate = " << time_gen << std::endl;
	auto	mtime = std::chrono::high_resolution_clock::now();
	auto	time_mesh = std::chrono::duration<float
			, std::chrono::seconds::period>(mtime - time).count();
	std::cout << "time to mesh = " << time_mesh << std::endl;
	std::cout << "total = " << time_mesh + time_gen << std::endl;
	m_vdb.mesh(m_mesh, box);
	return (0);
}
