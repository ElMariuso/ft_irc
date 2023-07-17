# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mthiry <mthiry@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/29 20:14:13 by root              #+#    #+#              #
#    Updated: 2023/07/17 15:11:08 by mthiry           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

GREEN = \033[0;32m
RED = \033[0;31m
WHITE = \033[0m
NC = \033[0m

NAME = ircserv
MAKE = make
SILENT = -s

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
		$(wildcard  $(SRC_DIR)/Utils.cpp) \
		$(wildcard  $(SRC_DIR)/Command.cpp)

OBJ_DIR	= objs
OBJ_FILES = $(notdir $(SRCS:.cpp=.o))
OBJS = $(addprefix $(OBJ_DIR)/,$(OBJ_FILES))

all:
	@echo "*************************************"
	@echo "*          BUILDING FT_IRC          *"
	@echo "*************************************"
	@echo ""
	@$(MAKE) $(SILENT) $(NAME)
	@echo ""
	@echo "*************************************"
	@echo "*          BUILD COMPLETE!          *"
	@echo "*************************************"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "   $(GREEN)[BUILD]:$(WHITE)	$<$(NC)"
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -c $< $(INCS) -o $@

$(NAME): $(OBJS)
	@echo "   $(GREEN)[COMPILING IRCSERV]$(WHITE)"
	@$(CPP) $(FLAGS) $(OBJS) -o $(NAME)

clean:
	@echo "*************************************"
	@echo "*          CLEANING FT_IRC          *"
	@echo "*************************************"
	@echo ""
	@echo "   $(RED)[DELETING OBJS]$(WHITE)"
	@$(RMF) $(OBJS)
	@$(RMF) $(OBJ_DIR)
	@echo ""
	@echo "*************************************"
	@echo "*          CLEAN COMPLETE!          *"
	@echo "*************************************"

fclean: clean
	@echo ""
	@echo "   $(RED)[DELETING IRCSERV]$(WHITE)"
	@$(RMF) $(NAME)
	@echo ""
	@echo "*************************************"
	@echo "*          IRCSERV CLEANED!         *"
	@echo "*************************************"

re: fclean all

.PHONY: all clean fclean re