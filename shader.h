/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/23 11:48:52 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/09 22:36:41 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define BUFF_SIZE	2048 

char	*shader_load(char* file, int *size_ret);
int		shader_create_module(VkDevice &device, const char *bytecode
	, int size, VkShaderModule *shader_module);
