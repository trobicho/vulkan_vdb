/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:30 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/28 20:09:10 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Map.h"
#include "Player.h"
#include "My_vulkan.h"
#include "Moore_accessor.h"

#define CHUNK_LOG_X		(6)
#define CHUNK_LOG_Y		(7)
#define CHUNK_LOG_Z		(6)

struct	s_chunk
{
	uint32_t	start_offset;
	uint32_t	end_offset;
	bool		in_vbo;
	s_vec3i		origin;
};

class	Map_loader
{
	public:
		Map_loader(Vdb_test &my_vdb, Player &player);
		~Map_loader(){};

		void			thread_loader();
		int				load_one_chunck(uint32_t x, uint32_t z);
		const Mesh&		get_mesh_ref() const {return (m_mesh);}
		const Vdb_test&	get_vdb_ref() const {return (m_vdb);}
		bool			has_update() const {return (m_update);}
		void			quit(){m_quit = true;}
		void			update();

	private:
		Vdb_test		&m_vdb;
		const Player	&m_player;
		Moore_accessor	m_moore_access;
		Mesh			m_mesh;
		Map				m_map;
		s_chunk			m_chunk;
		bool			m_update = false;
		bool			m_quit = false;
};
