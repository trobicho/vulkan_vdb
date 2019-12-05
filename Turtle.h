/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Turtle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/04 19:20:12 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/04 23:50:35 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.h"
#include <vector>
#include <string>
#include <stack>

#define A_DRAW			(1)
#define A_PUSH			(1 << 1)
#define A_POP			(1 << 2)
#define A_POP_BEFORE	(1 << 3)
#define A_APPLY_MAT		(1 << 4)
#define	A_SCALE_LEN		(1 << 5)

struct	s_action
{
	char		c;
	float		scalar_len;
	glm::mat4	matrix;
	int			flag;
};

struct	s_state
{
	glm::vec3	pos;
	glm::vec3	vec;
	glm::mat4	mat;
	glm::vec4	color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	float		len;
};

class	Turtle
{
	public:
		Turtle(Mesh &mesh);
		~Turtle(){};

		void		exec(std::string str, s_state &state);
		void		draw(s_state &state);
		void		add_action(char c, s_action &action);
		s_action*	get_action(char c);

	private:
		Mesh					&m_mesh;
		std::vector<s_action>	m_action;
		std::stack<s_state>		m_stack;
};
