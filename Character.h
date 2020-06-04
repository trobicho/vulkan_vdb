/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/03 13:12:18 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/04 08:19:14 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>

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

class	Character
{
	public:
		Character(int nb_feet);
		~Character(){};

		virtual int		get_state() = 0;
		std::vector<s_foot_info>&
					get_feet_info_ref() {return (m_feet_info);}
		s_feet_gate&	get_foot_gate_ref() {return (m_feet_gate);}

	private:
		void			step(int inc = 1);

		int				m_nb_feet;
		s_feet_gate		m_feet_gate;
		std::vector<s_foot_info>
						m_feet_info;
};
