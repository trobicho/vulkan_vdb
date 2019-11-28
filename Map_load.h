/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Load.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/28 16:17:38 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/28 16:56:44 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class	Map_loader
{
	public:
		Map_loader(Vdb_test &my_vdb);
		~Map_loader(){};

		int		load_one_chunck(uint32_t x, uint32_t z);
		Mesh&	get_mesh_ref() const {return (m_mesh);}
		Mesh&	get_vdb_ref() const {return (m_vdb);}

	private:
		Vdb_test		&m_vdb;
		Moore_accessor	m_moore_access;
		Mesh			m_mesh;
		Map				m_map(0);
};
