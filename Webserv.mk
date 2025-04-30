# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Webserv.mk                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/03 13:20:01 by maiboyer          #+#    #+#              #
#    Updated: 2025/04/30 18:03:22 by maiboyer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXXFLAGS_ADDITIONAL ?=
LDFLAGS_ADDITIONAL ?=

SUBJECT_URL		=	https://cdn.intra.42.fr/pdf/pdf/141614/en.subject.pdf

BASE_PATH		?=	$(shell pwd)
NAME			=	webserv

LIB_NAME		?=	
TARGET			=	./$(NAME)
CXX				?=	c++
CXXFLAGS		=	-Wall -Wextra -MMD -std=c++98
CXXFLAGS		+=	$(CXXFLAGS_ADDITIONAL)
LDFLAGS			=  $(LDFLAGS_ADDITIONAL)
# sorting + removing useless args
CXXFLAGS		:=	$(shell /bin/sh -c 'for arg in $(CXXFLAGS); do echo $$arg; done | sort -u')

-include 			./Filelist.mk

OBJ				=	$(addsuffix .o,$(addprefix $(BUILD_DIR)/,$(SRC_FILES) .flags))
DEPS			=	$(addsuffix .d,$(addprefix $(BUILD_DIR)/,$(SRC_FILES)))

INCLUDES		=	$(addprefix -I,$(foreach P,$(INCLUDE_DIR), $(realpath $(P))))
COL_BOLD		=	\033[1m
COL_GOLD		=	\033[93m
COL_RED			=	\033[31m
COL_GRAY		=	\033[90m
COL_GREEN		=	\033[32m
COL_RESET		=	\033[0m
COL_WHITE		=	\033[37m

ECHO			?=	/usr/bin/env echo

.PHONY = all bonus clean re subject filelist .clangd archive _flags _archive_inner

export BUILD_DIR
export CXX
export CXXFLAGS
export INCLUDES
export LDFLAGS

all:
	@$(MAKE) -f Webserv.mk _flags
	@$(MAKE) -f Webserv.mk $(NAME)

$(TARGET): $(OBJ)
	@$(ECHO) -e '$(COL_GRAY) Linking \t $(COL_GOLD)$(TARGET)$(COL_RESET)'
	@$(CXX) $(INCLUDES) $(OBJ) $(LDFLAGS) $(CXXFLAGS) -o $(NAME)

$(BUILD_DIR)/.flags.o: $(BUILD_DIR)/.flags.txt
	@mkdir -p $(dir $@)
	@$(ECHO) -e '$(COL_GRAY) Creating\t $(COL_GOLD).flags.o$(COL_RESET)'
	@cd $(BUILD_DIR) && objcopy --input-target binary \
		--output-target elf64-x86-64 \
		--binary-architecture i386:x86-64 \
		--rename-section .data=.rodata,CONTENTS,ALLOC,LOAD,READONLY,DATA \
		--redefine-sym=_binary__flags_txt_start=__flags_start \
		--redefine-sym=_binary__flags_txt_end=__flags_end \
		--redefine-sym=_binary__flags_txt_size=__flags_size \
		--add-section .note.GNU-stack=/dev/null \
		.flags.txt .flags.o


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_DIR)/.flags.txt
	@mkdir -p $(dir $@)
	@$(ECHO) -e '$(COL_GRAY) Building\t $(COL_GREEN)$<$(COL_RESET)'
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -DBASE_PATH=\"$(BASE_PATH)\" -c $< -o $@

clean:
	@$(ECHO) -e '$(COL_GRAY) Removing\t $(COL_RED)$(BUILD_DIR)$(COL_RESET)'
	@rm -rf $(BUILD_DIR)

fclean: clean
	@$(ECHO) -e '$(COL_GRAY) Removing\t $(COL_RED)$(NAME)$(COL_RESET)'
	@rm -f $(NAME)

re: fclean all

_archive_inner: $(OBJ)
archive: 
	@$(MAKE) -f Webserv.mk _flags
	@$(MAKE) -f Webserv.mk _archive_inner
	@echo -e '$(COL_GRAY) Bundle\t $(COL_GREEN)$(BUILD_DIR)/webserv.a$(COL_RESET)'
	@ar rcs $(BUILD_DIR)/webserv.a $(OBJ)

filelist:
	@rm -f Filelist.mk
	@printf '%-78s\\\n' "SRC_FILES =" > Filelist.mk
	@tree $(SRC_DIR) -ifF | rg '$(SRC_DIR)/(.*)\.cpp$$' --replace '$$1' | sed -re 's/^(.*)_([0-9]+)$$/\1|\2/g' | sort -t'|' --key=1,1 --key=2,2n | sed -e's/|/_/' | xargs printf '%-78s\\\n' >> Filelist.mk
	@echo "" >> Filelist.mk

.ONESHELL:
_flags: 
	@/bin/sh <<EOF
	#newlines have been replaced by spaces, and a space has been added at the end to satisfy compatibility
	if [ \
	"CXX = $$CXX CXXFLAGS = $$CXXFLAGS LDFLAGS = $$LDFLAGS " \
	!= "$$(cat $$BUILD_DIR/.flags.txt 2>/dev/null | tr '\n' ' ')" ]; then 
		mkdir -p $$BUILD_DIR;
		echo -e "CXX = $$CXX\nCXXFLAGS = $$CXXFLAGS\nLDFLAGS = $$LDFLAGS" > $$BUILD_DIR/.flags.txt; 
	fi;
	EOF


# change $(OBJ) with the name of all objects
# change $(TARGET) with the name of the executables
# you can modify this to change the default "classes" of symbols showed
SYMDIFF_CLASSES = other builtin allowed
# this can be overriten to add flags (-D to demangle, -A to unconditionnally add all classes)
SYMDIFF_FLAGS = -D
SYMDIFF_ALLOWED = accept access bind chdir close closedir connect dup dup2    \
				  epoll_create epoll_ctl epoll_wait errno execve fcntl fork   \
				  freeaddrinfo gai_strerror getaddrinfo getprotobyname        \
				  getsockname htonl htons kevent kill kqueue listen ntohl     \
				  ntohs open opendir pipe poll read readdir recv select send  \
				  setsockopt signal socket socketpair stat strerror waitpid   \
				  write

symdiff:
	@$(MAKE) -f ./Webserv.mk --no-print-directory all "CXXFLAGS=-g3 -Wall -Wextra -MMD -std=c++98" "CFLAGS=-g3 -Wall -Wextra -MMD"
	@./symdiff.py -C $(SYMDIFF_CLASSES) -o $(OBJ) -b $(TARGET) $(SYMDIFF_FLAGS) -a $(SYMDIFF_ALLOWED)

-include $(DEPS)
