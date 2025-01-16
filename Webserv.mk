# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Webserv.mk                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/03 13:20:01 by maiboyer          #+#    #+#              #
#    Updated: 2025/01/16 12:38:44 by maiboyer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXXFLAGS_ADDITIONAL ?=

SUBJECT_URL		=	https://cdn.intra.42.fr/pdf/pdf/141614/en.subject.pdf

BASE_PATH		?=	$(shell pwd)
NAME			=	webserv

LIB_NAME		?=	
TARGET			=	./$(NAME)
CXX				?=	c++
CXXFLAGS		=	-Wall -Werror -Wextra -MMD -std=c++98
CXXFLAGS		+=	$(CXXFLAGS_ADDITIONAL)

-include 			./Filelist.mk

OBJ				=	$(addsuffix .o,$(addprefix $(BUILD_DIR)/,$(SRC_FILES)))
DEPS			=	$(addsuffix .d,$(addprefix $(BUILD_DIR)/,$(SRC_FILES)))

INCLUDES		=	$(addprefix -I,$(foreach P,$(INCLUDE_DIR), $(realpath $(P))))
COL_BOLD		=	\033[1m
COL_GOLD		=	\033[93m
COL_RED			=	\033[31m
COL_GRAY		=	\033[90m
COL_GREEN		=	\033[32m
COL_RESET		=	\033[0m
COL_WHITE		=	\033[37m


.PHONY = all bonus clean re subject filelist .clangd
all: $(NAME)

#$(NAME): $(TARGET)

$(TARGET): $(OBJ)
	@echo -e '$(COL_GRAY) Linking \t $(COL_GOLD)$(TARGET)$(COL_RESET)'
	@$(CXX) $(INCLUDES) $(OBJ) $(CXXFLAGS) -o $(NAME)
	@#ar rcs $(BUILD_DIR)/$(NAME) $(OBJ)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo -e '$(COL_GRAY) Building\t $(COL_GREEN)$<$(COL_RESET)'
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo -e '$(COL_GRAY) Removing\t $(COL_RED)$(BUILD_DIR)$(COL_RESET)'
	@rm -rf $(BUILD_DIR)

fclean: clean
	@echo -e '$(COL_GRAY) Removing\t $(COL_RED)$(NAME)$(COL_RESET)'
	@rm -f $(NAME)

re: fclean all

filelist:
	@rm -f Filelist.mk
	@printf '%-78s\\\n' "SRC_FILES =" > Filelist.mk
	@tree $(SRC_DIR) -ifF | rg '$(SRC_DIR)/(.*)\.cpp$$' --replace '$$1' | sed -re 's/^(.*)_([0-9]+)$$/\1|\2/g' | sort -t'|' --key=1,1 --key=2,2n | sed -e's/|/_/' | xargs printf '%-78s\\\n' >> Filelist.mk
	@echo "" >> Filelist.mk

-include $(DEPS)
