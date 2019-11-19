/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Block.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/19 21:20:09 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/19 21:33:04 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Block.h"

glm::vec3	get_color_from_block_type(uint32_t block)
{
	switch (block)
	{
		case 1:
			return (glm::vec3(0.1f, 0.8f, 0.1f));
			break;
		case 2:
			return (glm::vec3(0.6f, 0.6f, 0.6f));
			break;
		case 3:
			return (glm::vec3(0.2f, 0.2f, 0.8f));
			break;
		default:
			return (glm::vec3(0.5f, 0.5f, 0.5f));
			break;
	}
}
