# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: root <root@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/29 20:14:13 by root              #+#    #+#              #
#    Updated: 2023/07/03 17:35:37 by root             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CPP = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm
RMF = rm -rf

INC_DIR = include
INCS = -I$(INC_DIR)

SRC_DIR = src
SRCS = $(wildcard  $(SRC_DIR)/main.cpp) \
		$(wildcard  $(SRC_DIR)/Channel.cpp) \
		$(wildcard  $(SRC_DIR)/Client.cpp) \
		$(wildcard  $(SRC_DIR)/Server.cpp) \
		$(wildcard  $(SRC_DIR)/Message.cpp)

OBJ_DIR	= objs
OBJ_FILES = $(notdir $(SRCS:.cpp=.o))
OBJS = $(addprefix $(OBJ_DIR)/,$(OBJ_FILES))

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CPP) $(FLAGS) -c $< $(INCS) -o $@

$(NAME): $(OBJS)
	$(CPP) $(FLAGS) $(OBJS) -o $(NAME)

clean:
	$(RMF) $(OBJS)
	$(RMF) $(OBJ_DIR)

fclean: clean
	$(RMF) $(NAME)

re: fclean all

.PHONY: all clean fclean re