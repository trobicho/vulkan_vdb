/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Block.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 21:20:01 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/09 09:47:00 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glm/glm.hpp>

enum	e_block_type
{
	bl_air = 0,
	bl_stone = 1,
	bl_bedrock,
	bl_dirt,
	bl_water,
	bl_grass,
	bl_ice,
	bl_sand,
	bl_wood,
	bl_leaf,
};

glm::vec4	get_color_from_block_type(uint32_t block);
