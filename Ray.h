/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/08 01:13:13 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/08 01:26:44 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Vdb_test.h"
#define		MAX_DIST	400.0
#define		MAX_STEP	150

class	Ray
{
	public:
		Ray(glm::vec3 ro, glm::vec3 rd);
		~Ray(){};

		int					launch(Vdb_test &vdb, int max_dist = MAX_DIST);
		void				step();
		int					step(const Node_v *node);
		inline double		get_dist() const {return (m_d);};
		inline s_vec3i		get_pos() const {return (m_pos);};
		inline glm::vec3	get_dir() const {return (m_rd);};
		inline glm::vec3	get_color() const {return (m_color);};
		inline int			get_side() const {return (m_side);};
		double				calc_dist();


	private:
		glm::vec3	m_ro;
		glm::vec3	m_rd;
		glm::vec3	m_delta_dist;
		glm::vec3	m_side_dist;
		s_vec3i		m_pos;
		s_vec3i		m_step;
		double		m_d = 0.0;
		int			m_side;
		glm::vec3	m_color = glm::vec3(0, 0, 0);
};
