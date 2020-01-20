# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: trobicho <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/11/02 20:38:42 by trobicho          #+#    #+#              #
#    Updated: 2019/12/24 19:56:10 by trobicho         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC 			=	g++
CXXFLAGS	=	-std=c++14 -g
NAME		=	vulkan_vdb

SRCS_PATH	=	./
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

SRCS_NAME	=	main_spider.cpp \
				Spider.cpp \
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
				Ray.cpp

HDRS_NAME	=	Vdb_test.h \
				Spider.h \
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
				Ray.cpp

OBJS_NAME	=	$(SRCS_NAME:.cpp=.o)

SRCS = $(addprefix $(SRCS_PATH)/, $(SRCS_NAME))
HDRS = $(addprefix $(HDRS_PATH)/, $(HDRS_NAME))
OBJS = $(addprefix $(OBJS_PATH)/, $(OBJS_NAME))

all: $(NAME)

$(NAME): $(SRCS) $(HDRS) $(OBJS) Makefile
	$(CC) $(CXXFLAGS) $(INCS_FLAGS) $(SDL_IFLAGS) $(SRCS) $(LDFLAGS) $(SDL_LFLAGS) -o $(NAME)

$(OBJS_PATH)/%.o: $(SRCS_PATH)/%.cpp $(HDRS) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@echo "\033[38;2;0;255;0m[cc]\033[0m: $< -> $@"
	@printf "\e[1A"
	@$(CC) $(CXXFLAGS) -I $(HDRS_PATH) $(INCS_FLAGS) $(SDL_IFLAGS) -c $< -o $@
	@printf "\e[0K"

clean:
	rm -f $(OBJS) $(GEN_OBJS) $(ENV_OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re logo test
