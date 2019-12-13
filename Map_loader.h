/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map_loader.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 17:47:30 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/12 21:33:49 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Map.h"
#include "Player.h"
#include "My_vulkan.h"
#include "Moore_accessor.h"
#include <chrono>
#include <array>
#include <cmath>
#include <mutex>

#define CHUNK_LOG_X		(4)
#define CHUNK_LOG_Y		(8)
#define CHUNK_LOG_Z		(4)

#define	MAX_CHUNK		1024

class	Map_loader
{
	public:
		Map_loader(Vdb_test &my_vdb, My_vulkan &vulk, Player &player);
		~Map_loader();

		void			thread_loader();
		void			block_change(s_vec3i block);
		int				load_pos(s_vec3i pos);
		int				generate_one_chunck(s_vbox &box);
		int				mesh_one_chunck(s_vbox &box);
		const Vdb_test&	get_vdb_ref() const {return (m_vdb);}
		bool			has_update() const {return (m_update);}
		void			quit(){m_quit = true;}
		int				update();
		inline int		taxicab_chunk_dist(s_vec3i center, s_vec3i pos)
		{
			int dx = abs((center.x >> CHUNK_LOG_X) - (pos.x >> CHUNK_LOG_X));
			int dz = abs((center.z >> CHUNK_LOG_Z) - (pos.z >> CHUNK_LOG_Z));

			return (dx + dz);
		}


	private:
		int				search_new_chunk();
		void			tag_unload_far_chunk();
		void			unload_far_chunk();
		int				mesh_one_chunck(s_vbox &box, s_chunk &chunk);
		int				next_chunk_in_radius(s_vec3i center
							, s_vec3i &pos, int radius);
		void			command_buffer_binder(VkCommandBuffer &cmd_buffer
							, VkDescriptorSet &desc_set
							, VkPipelineLayout &pipeline_layout);

		Vdb_test		&m_vdb;
		const Player	&m_player;
		My_vulkan		&m_vulk;
		Moore_accessor	m_moore_access;
		Map				m_map;
		//std::vector<s_chunk>
		t_chunk_cont	m_chunk;
		uint32_t		m_nb_chunk = 0;
		bool			m_update = false;
		bool			m_quit = false;
		size_t			m_old_size_vbo = 0;
		size_t			m_old_size_ibo = 0;
		uint32_t		m_meshing_radius = 8;
		uint32_t		m_unload_meshing_radius = 20;
		uint32_t		m_generate_radius = 20;
		int				m_need_unload = 0;
		std::mutex		m_mesh_mutex;
		std::mutex		m_update_mutex;
};
