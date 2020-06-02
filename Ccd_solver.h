/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ccd_solver.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/06 14:19:28 by trobicho          #+#    #+#             */
/*   Updated: 2020/05/26 16:22:18 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.h"

class	Ccd_solver
{
	public:
		Ccd_solver(){};
		static void	ccd_solve(std::vector<glm::mat4> &bones
			, std::vector<glm::vec3> &bones_pos, glm::vec3 target);
		static void	calc_matrix(std::vector<glm::mat4> &bones
			, std::vector<glm::vec3> &bones_pos, int id, glm::vec3 target);

	private:
		static void apply_matrix(std::vector<glm::mat4> &bones
				, std::vector<glm::vec3> &bones_pos, int id);
};
