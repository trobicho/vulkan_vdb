/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Moore_accessor.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/23 03:49:25 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/23 06:08:30 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include <bitset>

#define MOORE_UP		4
#define MOORE_DOWN		22
#define MOORE_FRONT		10
#define MOORE_BACK		16
#define MOORE_RIGHT		14
#define MOORE_LEFT		12

class	Moore_accessor
{
	public:
		Moore_accessor(Vdb_test &vdb);
		~Moore_accessor(){};

		void	find_neigh(s_vec3i v, Node_v *node);

		bool	operator[](std::size_t pos) const {return (m_neigh[pos]);}

	private:
		Vdb_test		&m_vdb;
		Node_v			*m_node_cur = nullptr;
		std::bitset<27>	m_neigh;
};
