/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/04 06:38:12 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/27 12:59:44 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Mesh.h"

struct	s_vbox
{
	s_vec3i	origin;
	s_vec3i	len;
};

template <class Value>
class	Node
{
	public:
		Node(){};
		inline bool			is_leaf() const {return (do_is_leaf());}
		inline s_vec3i		get_log() const {return (do_get_log());}
		inline s_vec3i		get_slog() const {return (do_get_slog());}
		inline s_vec3i		get_child_slog() const {return (do_get_child_slog());}
		inline const int	get_sSize() const {return (do_get_sSize());}
		inline void			set_vox(Value value, int32_t x, int32_t y, int32_t z)
		{
			return (do_set_vox(value, x, y, z));
		}
		inline Value		get_vox(int32_t x, int32_t y, int32_t z) const
		{
			return (do_get_vox(x, y, z));
		}
		inline const Node<Value>
							*get_interresting_node(s_vec3i v, Value &value) const
		{
			return (do_get_interresting_node(v, value));
		}
		inline void			mesh(Mesh &mesh) const {do_mesh(mesh);}
		inline void			mesh(Mesh &mesh, const s_vbox &box) const
								{do_mesh(mesh, box);}

	private:
		virtual bool		do_is_leaf() const = 0;
		virtual s_vec3i		do_get_log() const = 0;
		virtual s_vec3i		do_get_slog() const = 0;
		virtual s_vec3i		do_get_child_slog() const = 0;
		virtual const int	do_get_sSize() const = 0;
		virtual void		do_set_vox(Value value, int32_t x, int32_t y, int32_t z) = 0;
		virtual Value		do_get_vox(int32_t x, int32_t y, int32_t z) const = 0;
		virtual const Node<Value>
							*do_get_interresting_node(s_vec3i v, Value &value) const = 0;
		virtual void		do_mesh(Mesh &mesh) const = 0;
		virtual void		do_mesh(Mesh &mesh, const s_vbox &box) const = 0;
};
