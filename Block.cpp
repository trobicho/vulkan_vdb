/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Block.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 21:20:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/07 04:31:36 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Block.h"

glm::vec4	get_color_from_block_type(uint32_t block)
{
	switch ((e_block_type)block)
	{
		case bl_stone:
			return (glm::vec4(0.35f, 0.35f, 0.35f, 1.0f));
			break;
		case bl_bedrock:
			return (glm::vec4(0.5f, 0.137f, 0.027f, 1.0f));
			break;
		case bl_dirt:
			return (glm::vec4(0.48f, 0.35f, 0.25f, 1.0f));
			break;
		case bl_grass:
			return (glm::vec4(0.1f, 0.8f, 0.1f, 1.0f));
			break;
		case bl_water:
			return (glm::vec4(0.2f, 0.2f, 0.8f, 0.1f));
			break;
		case bl_ice:
			return (glm::vec4(0.65f, 0.65f, 0.8f, 1.0f));
			break;
		case bl_sand:
			return (glm::vec4(0.9f, 0.9f, 0.0f, 1.0f));
			break;
		case bl_wood:
			return (glm::vec4(0.48f, 0.35f, 0.25f, 1.0f));
			break;
		case bl_leaf:
			return (glm::vec4(0.1f, 0.8f, 0.1f, 1.0f));
			break;
		default:
			return (glm::vec4(0.7f, 0.1f, 0.1f, 1.0f));
			break;
	}
}
