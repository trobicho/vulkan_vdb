/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/03 13:12:18 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/11 10:56:35 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Skeleton_legged.h"

enum	e_foot_state {f_stance = 0, f_swing, f_new_swing};

#define		CHAR_STATE_FALLING		1 
#define		CHAR_STATE_WALKING		(1 << 1) 

struct	s_foot_info
{
	int			state;
	int			begin_gate;
	int			end_gate;

	bool		right_side = true;
	float		base_angle_torso;
	float		base_height;
	float		base_dist_torso;
	glm::vec3	target_world;
	glm::vec3	next_target_world;
	glm::vec3	vector_to_next_target;
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

		virtual	float		get_scale() const = 0;
		virtual	glm::vec3	get_pos() const = 0;
		virtual	glm::vec3	get_center() const = 0;
		virtual int			get_state() = 0;
		virtual void		ik_all() = 0;
		virtual void		foot_to_target_world(int foot_id
								, glm::vec3 target_world) = 0;
		virtual void		foot_to_target_relative(int foot_id
								, glm::vec3 target) = 0;
		void				step(int inc = 1);
		virtual void		move(glm::vec3 v) = 0;
		int					get_nb_feet() {return (m_nb_feet);}

		std::vector<s_foot_info>&
						get_feet_info_ref() {return (m_feet_info);}
		s_foot_info&	get_foot_info_ref(int foot_id)
							{return (m_feet_info[foot_id]);}
		s_feet_gate&	get_feet_gate_ref() {return (m_feet_gate);}
		glm::vec3&		get_foot_target_world_ref(int foot_id)
							{return (m_feet_info[foot_id].target_world);}
		void			set_foot_next_target_world(int foot_id, glm::vec3 target)
							{m_feet_info[foot_id].next_target_world = target;}
		glm::vec3		get_foot_next_target_world(int foot_id)
							{return (m_feet_info[foot_id].next_target_world);}
	protected:
		s_feet_gate					m_feet_gate;
		std::vector<s_foot_info>	m_feet_info;
		int							m_nb_feet;
};
