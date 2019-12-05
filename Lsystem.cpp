/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lsystem.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/04 19:26:20 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/05 00:59:03 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lsystem.h"
#include <iostream>
#include <glm/gtc/quaternion.hpp>

Lsystem::Lsystem(Mesh &mesh): m_turtle(Turtle(mesh))
{
}

static std::string
		b_2d(std::string str)
{
	std::string	res = "";

	for (char &c : str)
	{
		if (c == '1')
			res += "11";
		else if (c == '0')
			res += "1[0]0";
		else
			res += c;
	}
	return (res);
}

void	Lsystem::basic()
{
	std::string	str = "0";
	s_state		state;
	s_action	action;

	for (int iter = 0; iter < 10; iter++)
	{
		str = b_2d(str);
	}
	action.flag = A_DRAW;
	m_turtle.add_action('0', action);
	action.flag = A_DRAW;
	m_turtle.add_action('1', action);
	action.flag = A_PUSH | A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(-45.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('[', action);
	action.flag = A_POP_BEFORE | A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(45.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action(']', action);

	state.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	state.vec = glm::vec3(0.0f, 1.0f, 0.0f);
	state.len = 10.0f;
	m_turtle.exec(str, state);
}

static std::string
		b_3d(std::string str)
{
	std::string	res = "";

	for (char &c : str)
	{
		if (c == '1')
			res += "11";
		else if (c == '0')
			res += "1A0B0C0";
		else
			res += c;
	}
	return (res);
}

void	Lsystem::basic3d()
{
	std::string	str = "0";
	s_state		state;
	s_action	action;

	for (int iter = 0; iter < 10; iter++)
	{
		//std::cout << iter << ": " << str << std::endl;
		str = b_3d(str);
	}
	//std::cout << str << std::endl;
	action.flag = A_DRAW;
	m_turtle.add_action('0', action);
	action.flag = A_DRAW;
	m_turtle.add_action('1', action);

	action.flag = A_PUSH | A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(-45.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('A', action);

	action.flag = A_POP_BEFORE | A_PUSH | A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(120.0f)
					, glm::vec3(0.0f, 1.0f, 0.0f)))
					* glm::mat4_cast(glm::angleAxis(glm::radians(-45.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('B', action);
	action.flag = A_POP_BEFORE | A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(240.0f)
					, glm::vec3(0.0f, 1.0f, 0.0f)))
					* glm::mat4_cast(glm::angleAxis(glm::radians(-45.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('C', action);

	state.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	state.vec = glm::vec3(0.0f, 1.0f, 0.0f);
	state.len = 0.1f;
	state.mat = glm::mat4(1.0f);
	m_turtle.exec(str, state);
}

static std::string
		pl_2d(std::string str)
{
	std::string	res = "";

	for (char &c : str)
	{
		if (c == 'X')
			res += "F+[[X]-X]-F[-FX]+X";
		else if (c == 'F')
			res += "FF";
		else
			res += c;
	}
	return (res);
}

void	Lsystem::plant2d()
{
	std::string	str = "X";
	s_state		state;
	s_action	action;

	for (int iter = 0; iter < 10; iter++)
	{
		//std::cout << iter << ": " << str << std::endl;
		str = pl_2d(str);
	}
	//std::cout << str << std::endl;
	action.flag = A_DRAW;
	m_turtle.add_action('F', action);

	action.flag = A_PUSH;
	m_turtle.add_action('[', action);
	action.flag = A_POP;
	m_turtle.add_action(']', action);

	action.flag = A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(25.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('+', action);
	action.flag = A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(-25.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('-', action);

	state.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	state.vec = glm::vec3(0.0f, 1.0f, 0.0f);
	state.len = 0.1f;
	state.mat = glm::mat4(1.0f);
	m_turtle.exec(str, state);
}

static std::string
		pl_3d(std::string str)
{
	std::string	res = "";

	for (char &c : str)
	{
		if (c == 'X')
			res += "F+[[X]-X]a[[X]-X]b[[X]-X]-F[-FX]+XaXbX";
		else if (c == 'F')
			res += "FF";
		else
			res += c;
	}
	return (res);
}

void	Lsystem::plant3d()
{
	std::string	str = "X";
	s_state		state;
	s_action	action;

	for (int iter = 0; iter < 4; iter++)
	{
		//std::cout << iter << ": " << str << std::endl;
		str = pl_3d(str);
	}
	action.flag = A_DRAW;
	m_turtle.add_action('F', action);

	action.flag = A_PUSH;
	m_turtle.add_action('[', action);
	action.flag = A_POP;
	m_turtle.add_action(']', action);

	action.flag = A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(25.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('+', action);
	action.flag = A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(120.0f)
					, glm::vec3(0.0f, 1.0f, 0.0f)))
					* glm::mat4_cast(glm::angleAxis(glm::radians(25.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('a', action);
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(240.0f)
					, glm::vec3(0.0f, 1.0f, 0.0f)))
					* glm::mat4_cast(glm::angleAxis(glm::radians(25.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('b', action);
	action.flag = A_APPLY_MAT;
	action.matrix = glm::mat4_cast(glm::angleAxis(glm::radians(-25.0f)
					, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_turtle.add_action('-', action);

	state.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	state.vec = glm::vec3(0.0f, 1.0f, 0.0f);
	state.len = 0.1f;
	state.mat = glm::mat4(1.0f);
	m_turtle.exec(str, state);
}
