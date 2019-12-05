/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Turtle.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/04 19:19:42 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/04 23:53:01 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Turtle.h"

Turtle::Turtle(Mesh &mesh): m_mesh(mesh)
{
}

void		Turtle::exec(std::string str, s_state &state)
{
	s_action	*action;

	//m_stack.erase();
	for (char &c : str)
	{
		if ((action = get_action(c)) == nullptr)
			continue;
		if (action->flag & A_POP_BEFORE)
		{
			state = m_stack.top();
			m_stack.pop();
		}
		if (action->flag & A_PUSH)
			m_stack.push(state);
		if (action->flag & A_APPLY_MAT)
			state.mat *= action->matrix;
		if (action->flag & A_SCALE_LEN)
			state.len *= action->scalar_len;
		if (action->flag & A_DRAW)
			draw(state);
		if (action->flag & A_POP)
		{
			state = m_stack.top();
			m_stack.pop();
		}
	}
}

void		Turtle::draw(s_state &state)
{
	s_vertex	vertex(state.pos, state.color);
	m_mesh.add_vertex_with_basic_index(vertex);
	glm::vec3 vec = state.mat * glm::vec4(state.vec, 1.0f);
	state.pos += vec * state.len;
	vertex.pos = state.pos;
	m_mesh.add_vertex_with_basic_index(vertex);
}

void		Turtle::add_action(char c, s_action &action)
{
	action.c = c;
	m_action.push_back(action);
}

s_action*	Turtle::get_action(char c)
{
	for (s_action &action : m_action)
	{
		if (action.c == c)
			return (&action);
	}
	return (nullptr);
}
