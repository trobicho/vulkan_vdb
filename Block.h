/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Block.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 21:20:01 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/20 02:48:24 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glm/glm.hpp>

enum	e_block_type
{
	bl_stone = 1,
	bl_dirt = 2,
	bl_water,
	bl_grass,
	bl_ice,
	bl_sand,
};

glm::vec3	get_color_from_block_type(uint32_t block);
