/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lsystem.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/04 19:26:25 by trobicho          #+#    #+#             */
/*   Updated: 2019/12/05 00:46:15 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Turtle.h"

class	Lsystem
{
	public:
		Lsystem(Mesh &mesh);
		~Lsystem(){};

		void	basic();
		void	basic3d();
		void	plant2d();
		void	plant3d();

	private:
		Turtle		m_turtle;
};
