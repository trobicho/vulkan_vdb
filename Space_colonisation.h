/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Space_colonisation.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 01:42:47 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/07 04:40:09 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vdb_test.h"
#include <vector>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class	Branch
{
	public:
		Branch(){};
		Branch(glm::vec3 _pos, int _link = 0): pos(_pos), link(_link){};
		glm::vec3	pos;
		size_t		link;
		int			width = 1;

		bool		can_grow() {return (m_grow_count > 0);}
		bool		has_stop() {return (m_has_stop);}
		void		stop() {m_has_stop = true;}
		bool		has_infuance() {return (m_has_influance);}
		bool		has_grow() {return (m_has_grow);}
		void		is_influance() {m_has_influance = true;}
		Branch		grow();
		void		add_to_vec(glm::vec3 v)
		{
			m_grow_vec += v;
			m_grow_count++;
		}
		void		reset()
		{
			m_grow_count = 0;
			m_has_influance = false;
			m_grow_vec = glm::vec3(0.f, 0.f, 0.f);
		}

	private:
		int			m_grow_count = 0;
		glm::vec3	m_grow_vec;
		bool		m_has_stop = false;
		bool		m_has_influance = false;
		bool		m_has_grow = false;
};

struct	s_space_col_info
{
	float					di;
	float					dk;
	std::vector<glm::vec3>	attractor;
	std::vector<Branch>		branch;
};

class	Space_colonisation
{
	public:
		Space_colonisation(s_space_col_info &info, Vdb_test &vdb);
		~Space_colonisation(){};

		void					grow_full(int nb_step);
		void					add_leaf(Branch &branch);
		void					step();
		std::vector<Branch>		&get_branch(){return (m_branch);}
		std::vector<glm::vec3>  &get_attractor(){return (m_attractor);}

	private:
		std::vector<Branch>		&m_branch;
		std::vector<glm::vec3>	&m_attractor;
		float					m_di;
		float					m_dk;
		bool					m_first = true;
		Vdb_test				&m_vdb;
};
