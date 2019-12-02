/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:30 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/02 09:47:28 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Map.h"
#include "Player.h"
#include "My_vulkan.h"
#include "Moore_accessor.h"
#include <chrono>
#include <array>

#define CHUNK_LOG_X		(6)
#define CHUNK_LOG_Y		(7)
#define CHUNK_LOG_Z		(6)

#define	MAX_CHUNK		1024

struct	s_chunk
{
	uint32_t	start_offset;
	uint32_t	start_offset_idx;
	uint32_t	size;
	uint32_t	size_idx;
	bool		in_vbo;
	s_vec3i		origin;
};

class	Map_loader
{
	public:
		Map_loader(Vdb_test &my_vdb, My_vulkan &vulk, Player &player);
		~Map_loader(){};

		void			thread_loader();
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
		Mesh			m_mesh;
		Map				m_map;
		std::array<s_chunk, MAX_CHUNK>
						m_chunk;
		uint32_t		m_nb_chunk = 0;
		bool			m_update = false;
		bool			m_quit = false;
		size_t			m_old_size_vbo = 0;
		size_t			m_old_size_ibo = 0;
};
