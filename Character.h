/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/03 13:12:18 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/05 09:57:16 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Skeleton_legged.h"

enum	e_foot_state {f_stance = 0, f_swing, f_new_swing};

#define		CHAR_STATE_FALLING		1 
#define		CHAR_STATE_WALKING		(1 << 1) 

struct	s_foot_info
{
	int		state;
	int		begin_gate;
	int		end_gate;
};

struct	s_feet_gate
{
	int		cur = 0;
	int		len;
};

class	Character: public Skeleton_legged
{
	public:
		Character(int nb_feet);
		~Character(){};

		virtual int		get_state() = 0;
		virtual void	foot_to_target_world(int foot_id
							, glm::vec3 target_world) = 0;
		void			step(int inc = 1);

		std::vector<s_foot_info>&
						get_feet_info_ref() {return (m_feet_info);}
		s_feet_gate&	get_foot_gate_ref() {return (m_feet_gate);}
		const std::vector<glm::vec3>&
						get_feet_target_world_ref()
							{return (m_feet_target_world);}
		glm::vec3		get_foot_target_world(int foot_id)
							{return (m_feet_target_world[foot_id]);}
	protected:
		std::vector<glm::vec3>		m_feet_target_world;
		s_feet_gate					m_feet_gate;
		std::vector<s_foot_info>	m_feet_info;
};
