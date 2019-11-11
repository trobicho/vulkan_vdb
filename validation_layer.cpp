/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_layer.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 16:28:15 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/09 19:33:31 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

int		val_layer_check(char **val_layer, int nb_layer)
{
	int					i;
	uint32_t			a;
	uint32_t			layer_count;
	int					layer_found;
	VkLayerProperties	*available_layers;

	vkEnumerateInstanceLayerProperties(&layer_count, NULL);
	if ((available_layers = (VkLayerProperties*)
			malloc(sizeof(VkLayerProperties) * layer_count)) == NULL)
		return (0);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
	printf("layer count = %d, search(%d)\n", layer_count, nb_layer);
	i = 0;
	while (i < nb_layer)
	{
		layer_found = 0;
		a = 0;
		while (a < layer_count && layer_found == 0)
		{
			printf("%s\n", available_layers[a].layerName);
			if (strcmp(val_layer[i], available_layers[a].layerName) == 0)
				layer_found = 1;
			a++;
		}
		if (!layer_found)
			return (0);
		i++;
	}
	return (1);
}

char**	get_extensions(int debug, uint32_t *ext_count)
{
	int			i;
	const char	**glfw_ext;
	uint32_t	glfw_ext_count;
	char		**ext;

	glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_ext_count);
	*ext_count = glfw_ext_count;
	if (debug)
	{
		*ext_count += 1;
		ext = (char**)malloc(sizeof(char*) * *ext_count);
		i = 0;
		while (i < glfw_ext_count)
		{
			ext[i] = (char*)glfw_ext[i];
			i++;
		}
		ext[i] = (char*)malloc(sizeof(char) * 100);
		strcpy(ext[i], VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		return (ext);
	}
	return ((char**)glfw_ext);
}
