/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Skeleton_legged.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/03 13:05:24 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/03 13:07:30 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Skeleton_legged.h"

Skeleton_legged::Skeleton_legged(glm::vec3 torso_center)
{
	m_bones.resize(1, glm::mat4(1.0f));
	m_bones_pos.resize(1);
	m_bones_pos[0] = torso_center;
}
