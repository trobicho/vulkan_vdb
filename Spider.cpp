/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 22:08:47 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/22 00:29:10 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Spider.h"

Spider::Spider(): m_mesh(m_moore_access)
{
}

void	Spider::generate()
{
	m_vdb.set_vox(1, s_vec3i(0, 0, 0));
	m_vdb.mesh(m_mesh);
}
