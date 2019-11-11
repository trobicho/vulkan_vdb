/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_layer.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 16:27:58 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/09 22:37:29 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

int	val_layer_check(char **val_layer, int nb_layer);
char**	get_extensions(int debug, uint32_t *ext_count);
