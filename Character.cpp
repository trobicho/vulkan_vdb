/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/04 08:02:35 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/04 08:22:25 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character.h"

Character::Character(int nb_feet): m_nb_feet(nb_feet)
{
	s_foot_info		feet_inf;

	feet_inf.state = f_stance;
	feet_inf.begin_gate = 10;
	feet_inf.end_gate = 100;
	m_feet_info.resize(nb_feet, feet_inf);
	m_feet_gate.len = 256;
}

void	Character::step(int inc)
{
	m_feet_gate.cur += inc;
	if (m_feet_gate.cur >= m_feet_gate.len)
		m_feet_gate.cur = m_feet_gate.cur - m_feet_gate.len;
	for (int i = 0; i < m_nb_feet; i++)
	{
		if (m_feet_gate.cur >= m_feet_info[i].begin_gate
			&& m_feet_gate.cur <= m_feet_info[i].end_gate)
		{
			if (m_feet_info[i].state == f_stance)
				m_feet_info[i].state = f_new_swing;
			else
				m_feet_info[i].state = f_swing;
		}
		else
			m_feet_info[i].state = f_stance;
	}
}
