# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: trobicho <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/11/02 20:38:42 by trobicho          #+#    #+#              #
#    Updated: 2020/06/15 13:24:01 by trobicho         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC 			=	g++
CXXFLAGS	=	-std=c++14 -g
NAME		=	vulkan_vdb

SRCS_PATH	=	./
SRCS_UI_PATH	=	./imgui
HDRS_PATH	=	./
OBJS_PATH	=	./obj


ifeq ($(UNAME), Linux)
	INCS_FLAGS	= -I../my_lib_cpp/ -I$(VULKAN_SDK)/include \
		-I$(GLFW3_PATH)/include -I$(GLM_PATH)/include \
		-I $(INCLUDE_PATH)
	LDFLAGS = -L$(VULKAN_SDK)/lib `pkg-config --static --libs glfw3` -lvulkan -lm -L ../my_lib_cpp/ -ltrl
else
	INCS_FLAGS	= -I../QGen/my_lib_cpp/ -I$(VULKAN_SDK)/include \
		-I$(GLFW3_PATH)/include -I$(GLM_PATH)/include \
		-I $(INCLUDE_PATH)
	LDFLAGS = -L$(GLFW3_PATH)/lib -L$(VULKAN_SDK)/lib `pkg-config --static --libs glm`\
		-lvulkan -lglfw -lm -L ../QGen/my_lib_cpp/ -ltrl
endif

SRCS_UI_NAME	=	imgui.cpp \
					imgui_impl_vulkan.cpp \
					imgui_impl_glfw.cpp \
					imgui_draw.cpp \
					imgui_widgets.cpp \
					imgui_demo.cpp \

HDRS_UI_NAME	=	imgui.h \
					imgui_impl_vulkan.h \
					imgui_impl_glfw.h \
					imgui_internal.h \
					imstb_rectpack.h \
					imstb_textedit.h \
					imstb_truetype.h \
					imconfig.h \

SRCS_NAME	=	main.cpp \
				Dev_gui.cpp \
				Spider.cpp \
				Character.cpp \
				Character_controller.cpp \
				Feet_planner_default.cpp \
				Feet_planner.cpp \
				Enemy_vbo.cpp \
				Enemy_manager.cpp \
				Space_colonisation.cpp \
				Vdb_test.cpp \
				My_vulkan.cpp \
				init_swap_chain.cpp \
				gpu_pipeline.cpp \
				shader.cpp \
				validation_layer.cpp \
				Player.cpp \
				Physic.cpp \
				key_call.cpp \
				Map.cpp \
				perlin.cpp \
				Noise.cpp \
				Block.cpp \
				Moore_accessor.cpp \
				Map_loader.cpp \
				Chunk.cpp \
				Ccd_solver.cpp \
				Ray.cpp

HDRS_NAME	=	Vdb_test.h \
				Dev_gui.h \
				Spider.h \
				Character.h \
				Character_controller.h \
				Feet_planner_default.h \
				Feet_planner.h \
				Enemy_manager.h \
				Space_colonisation.h \
				libvdb.h \
				Root_node.h \
				Internal_node.h \
				Leaf_node.h \
				Node.h \
				My_vulkan.h \
				init_swap_chain.h \
				gpu_pipeline.h \
				shader.h \
				validation_layer.h \
				Player.h \
				Physic.h \
				Physic_entity.h \
				key_call.h \
				Map.h \
				perlin.h \
				Noise.h \
				Block.h \
				Moore_accessor.h \
				Map_loader.h \
				Mesh_interface.h \
				Mesh.h \
				Vertex.h \
				Ccd_solver.h \
				Ray.cpp

OBJS_NAME	=	$(SRCS_NAME:.cpp=.o) $(SRCS_UI_NAME:.cpp=.o) 

SRCS = $(addprefix $(SRCS_PATH)/, $(SRCS_NAME))
SRCS_UI = $(addprefix $(SRCS_UI_PATH)/, $(SRCS_UI_NAME))
HDRS_UI = $(addprefix $(SRCS_UI_PATH)/, $(HDRS_UI_NAME))
HDRS = $(addprefix $(HDRS_PATH)/, $(HDRS_NAME))
OBJS = $(addprefix $(OBJS_PATH)/, $(OBJS_NAME))

all: $(NAME)

$(NAME): $(SRCS) $(HDRS) $(SRCS_UI) $(HDRS_UI) $(OBJS) Makefile
	$(CC) $(CXXFLAGS) $(INCS_FLAGS) $(SRCS) $(SRCS_UI) $(LDFLAGS) -o $(NAME)

$(OBJS_PATH)/%.o: $(SRCS_PATH)/%.cpp $(HDRS) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@echo "\033[38;2;0;255;0m[cc]\033[0m: $< -> $@"
	@printf "\e[1A"
	@$(CC) $(CXXFLAGS) -I $(HDRS_PATH) $(INCS_FLAGS) -c $< -o $@
	@printf "\e[0K"

$(OBJS_PATH)/%.o: $(SRCS_UI_PATH)/%.cpp $(HDRS_UI) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@echo "\033[38;2;0;255;0m[cc]\033[0m: $< -> $@"
	@printf "\e[1A"
	@$(CC) $(CXXFLAGS) -I $(SRCS_UI_PATH) $(INCS_FLAGS) -c $< -o $@
	@printf "\e[0K"


clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
