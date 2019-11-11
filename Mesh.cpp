/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 07:00:55 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/11 09:57:29 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.h"

Mesh::Mesh()
{
}

void	Mesh::reset()
{
	vertex_buffer.clear();
	index_buffer.clear();
}

int		Mesh::add_vertex(s_vertex v)
{
	vertex_buffer.push_back(v);
	return (vertex_buffer.size() - 1);
}

int		Mesh::add_vertex_with_basic_index(s_vertex v)
{
	vertex_buffer.push_back(v);
	index_buffer.push_back(vertex_buffer.size() - 1);
	return (vertex_buffer.size() - 1);
}

void	Mesh::add_index(uint32_t idx)
{
	index_buffer.push_back(idx);
}
