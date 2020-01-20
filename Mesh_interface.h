/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh_interface.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/24 19:44:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/24 20:06:10 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

struct	s_vec3i
{
	s_vec3i(){};
	s_vec3i(int32_t sx, int32_t sy, int32_t sz): x(sx), y(sy), z(sz){};
	int32_t	x, y, z;
};

class	Mesh_interface
{
	public:
		virtual void		reset() = 0;
		virtual uint32_t	get_nb_vertex() const = 0;
		virtual uint32_t	get_nb_index() const = 0;
		virtual void		remove_vertex(uint32_t offset, uint32_t size) = 0;
		virtual void		add_big_cube_from_node(s_vec3i v
								, uint32_t type, void *node_ptr) = 0;
		virtual void		add_cube_from_node(s_vec3i v
								, uint32_t type, void *node_ptr) = 0;
};
