/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:30 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/08 07:26:32 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Map.h"
#include "Player.h"
#include "My_vulkan.h"
#include "Moore_accessor.h"
#include <chrono>
#include <array>

#define CHUNK_LOG_X		(5)
#define CHUNK_LOG_Y		(8)
#define CHUNK_LOG_Z		(5)

#define	MAX_CHUNK		1024

struct	s_chunk
{
	s_chunk(Moore_accessor &moore_access): mesh(moore_access);
	int			update(My_vulkan &vulk);

	Mesh			mesh;
	bool			in_vbo;
	bool			need_remesh = false;
	s_vec3i			origin;
	VkBuffer		vertex_buffer;
	VkDeviceMemory	vertex_buffer_memory;
	VkBuffer		vertex_index_buffer;
	VkDeviceMemory	vertex_index_buffer_memory;
};

class	Map_loader
{
	public:
		Map_loader(Vdb_test &my_vdb, My_vulkan &vulk, Player &player);
		~Map_loader(){};

		void			thread_loader();
		int				load_pos(s_vec3i pos);
		int				generate_one_chunck(s_vbox &box);
		int				mesh_one_chunck(s_vbox &box);
		const Mesh&		get_mesh_ref() const {return (m_mesh);}
		const Vdb_test&	get_vdb_ref() const {return (m_vdb);}
		bool			has_update() const {return (m_update);}
		void			quit(){m_quit = true;}
		int				update();

	private:
		int				mesh_one_chunck(s_vbox &box, uint32_t chunk_id);

		Vdb_test		&m_vdb;
		const Player	&m_player;
		My_vulkan		&m_vulk;
		Moore_accessor	m_moore_access;
		Map				m_map;
		std::vector<s_chunk>
						m_chunk;
		uint32_t		m_nb_chunk = 0;
		bool			m_update = false;
		bool			m_quit = false;
		size_t			m_old_size_vbo = 0;
		size_t			m_old_size_ibo = 0;
};
