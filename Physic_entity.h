/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Physic_entity.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/25 12:29:09 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/25 15:19:32 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"

struct	s_hitbox
{
	s_hitbox(){};
	s_hitbox(float sw, float sh): w(sw), h(sh){};
	float	x = 0.f, y = 0.f;
	float	w, h;
};

class Physic_entity
{
	public:
		Physic_entity(){};
		virtual ~Physic_entity(){};

		virtual bool		check_ground(const Vdb_test &world) = 0;
		virtual s_hitbox	get_hitbox() const = 0;
		virtual glm::vec3&	get_velocity_vec_ref() = 0;
		virtual glm::vec3	get_pos() const = 0;
		virtual void		set_pos(glm::vec3 pos) = 0;
};
