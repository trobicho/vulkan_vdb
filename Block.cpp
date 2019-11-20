/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Block.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 21:20:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/20 03:31:42 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Block.h"

glm::vec3	get_color_from_block_type(uint32_t block)
{
	switch ((e_block_type)block)
	{
		case bl_stone:
			return (glm::vec3(0.35f, 0.35f, 0.35f));
			break;
		case bl_dirt:
			return (glm::vec3(0.4f, 0.2f, 0.0f));
			break;
		case bl_grass:
			return (glm::vec3(0.1f, 0.8f, 0.1f));
			break;
		case bl_water:
			return (glm::vec3(0.2f, 0.2f, 0.8f));
			break;
		case bl_ice:
			return (glm::vec3(0.65f, 0.65f, 0.8f));
			break;
		case bl_sand:
			return (glm::vec3(0.9f, 0.9f, 0.0f));
			break;
		default:
			return (glm::vec3(0.7f, 0.1f, 0.1f));
			break;
	}
}
