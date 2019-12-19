/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Enemy.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/19 16:17:13 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/19 16:22:20 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Vdb_test.h"

using	Leaf_enemy = Leaf_node<uint32_t, 2>;
using	Internal_enemy = Internal_node<uint32_t, Leaf_enemy, 4>;

class	Enemy_vdb
{
	public:
		Enemy_vdb();
		~Enemy_vdb(){};

		void		set_vox(uint32_t value, s_vec3i v);
		void		unset_vox(s_vec3i v);
		uint32_t	get_vox(s_vec3i v) const;
		int			remove_node_by_slog(s_vec3i node_pos, uint32_t slog);
		const Node_v
					*get_interresting_node(s_vec3i v, uint32_t &value) const;
		void		pruning();
		void		mesh(Mesh &mesh) const;
		void		mesh(Mesh &mesh, const s_vbox &box) const;

	private:
		Internal_enemy	m_root_static = Internal_L1(0, 0, 0);
		s_vec3i			m_min;
		s_vec3i			m_max;
};

class	Enemy
{
	public:
		Enemy(){};
		~Enemy(){};
};
