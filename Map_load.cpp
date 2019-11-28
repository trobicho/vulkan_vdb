/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Load.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 16:17:36 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/28 16:57:06 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Map_loader.h"
#include "iostream.h"

Map_loader::Map_loader(Vdb_test &my_vdb): m_vdb(my_vdb)
					, m_moore_access(m_vdb), m_map(0)
{
	mesh.reset();
}

int		Map_loader::load_one_chunck(uint32_t x, uint32_t z)
{
	s_vbox		box;

	box.len = s_vec3i(256, 128, 256);
	box.origin = s_vec3i(x, 0, z);
	if (m_map.generate(m_vdb, box))
		return (1);
	my_vdb.pruning();
	m_vdb.mesh(mesh, box);
}

/*
	auto	time = std::chrono::high_resolution_clock::now();
	box.len = s_vec3i(256, 128, 256);
	auto	new_time = std::chrono::high_resolution_clock::now();
	auto	time_sec = std::chrono::duration<float
			, std::chrono::seconds::period>(new_time - time).count();
	time = new_time;
	my_vdb.pruning();
	std::cout << "time to generate = " << time_sec << std::endl;
	new_time = std::chrono::high_resolution_clock::now();
	time_sec = std::chrono::duration<float
			, std::chrono::seconds::period>(new_time - time).count();
	std::cout << "time to mesh = " << time_sec << std::endl;
	//mesh.remove_vertex(100000, 30000);
	//box.origin = s_vec3i(xr - box.len.x / 2, 0, zr - box.len.z / 2);
	std::cout << "total of " << map.get_nb_vox() << " voxels." << std::endl;
	std::cout << "total of " << mesh.get_nb_vertex() << " vertex." << std::endl;
	std::cout << "total of " << mesh.get_nb_index() << " index." << std::endl;
	std::cout << std::endl;
*/
