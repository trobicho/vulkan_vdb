/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/23 11:49:19 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/09 22:36:25 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "shader.h"

#include <string.h>
#include <stdio.h>

char	*shader_load(char* file, int *size_ret)
{
	int		fd;
	int		size;
	int		size_mod;
	int		ret_size;
	char	buf[BUFF_SIZE];
	char	*ret;
	char	*ret_save;

	if ((fd = open(file, O_RDONLY)) == -1)
		return (NULL);
	ret_size = 0;
	while ((size = read(fd, buf, BUFF_SIZE)) > 0)
	{
		size_mod = size;
		if (size % 4)
		{
			size_mod += size % 4;
			printf("resize\n");
		}
		ret_save = ret;
		if ((ret = (char*)malloc(sizeof(char) * (ret_size + size_mod))) == NULL)
			return (NULL);
		if (ret_size)
			memcpy(ret, ret_save, ret_size);
		memcpy(&ret[ret_size], buf, size);
		if (size_mod != size)
			bzero(&ret[ret_size + size], size % 4);
		ret_size += size_mod;
	}
	if (size_ret != NULL)
		*size_ret = ret_size;
	return (ret);
}

int		shader_create_module(VkDevice &device, const char *bytecode
	, int size, VkShaderModule *shader_module)
{
	VkShaderModuleCreateInfo	shader_info;

	shader_info = (VkShaderModuleCreateInfo){};
	shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_info.codeSize = size;
	printf("codeSize = %d\n", shader_info.codeSize);
	shader_info.pCode = (const uint32_t*)bytecode;
	/*-----------------------------------
	printf("----------------------------\n");
	int i;
	char *buf = (char*)shader_info.pCode;
	for (i = 0; i < size + size % 4; i++)
	{
		printf("%c", buf[i]);
	}
	printf("----------------------------\n");
	//-----------------------------------
	*/
	if (vkCreateShaderModule(device, &shader_info, NULL, shader_module)
		!= VK_SUCCESS)
	{
		printf("failed to create module!\n");
		return (-1);
	}
	return (0);
}
