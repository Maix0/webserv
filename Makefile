# **************************************************************************** #make
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rparodi <rparodi@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/12 11:05:05 by rparodi           #+#    #+#              #
#    Updated: 2025/02/28 21:41:28 by maiboyer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Objdir
BUILD_DIR		= $(shell realpath ./build)
SRC_DIR			=	./src
INCLUDE_DIR		=	./include

CXX=c++

SUBJECT_URL						= https://cdn.intra.42.fr/pdf/pdf/140049/en.subject.pdf
SUBJECT_URL_CGI_TESTER			= https://cdn.intra.42.fr/document/document/27456/cgi_tester
SUBJECT_URL_TESTER				= https://cdn.intra.42.fr/document/document/27454/tester
SUBJECT_URL_UBUNTU_CGI_TESTER	= https://cdn.intra.42.fr/document/document/27455/ubuntu_cgi_tester
SUBJECT_URL_UBUNTU_TESTER		= https://cdn.intra.42.fr/document/document/27457/ubuntu_tester

# Colors
GREEN = \033[32m
CYAN = \033[36m
GREY = \033[0;90m
RED = \033[0;31m
GOLD = \033[38;5;220m
END = \033[0m
BOLD = \033[1m
ITALIC = \033[3m
UNDERLINE = \033[4m

CXXFLAGS_ADDITIONAL=
# PMAKE_DISABLE =
PMAKE =
ifndef PMAKE_DISABLE
ifeq ($(shell uname), Linux)
	PMAKE = -j$(shell grep -c ^processor /proc/cpuinfo)
	#CFLAGS_ADDITIONAL	+= -DPRINT_BACKTRACE
endif
ifeq ($(shell uname), Darwin)
	PMAKE = -j$(shell sysctl -n hw.ncpu)
	#CFLAGS_ADDITIONAL	+= -DNVALGRIND
endif
endif



ifeq ($(MAKECMDGOALS), bonus)
    CFLAGS_ADDITIONAL += -DBONUS=1
    BUILD_DIR := $(BUILD_DIR)/bonus
endif

NAME=webserv
# TODO: REMOVE THIS WHEN FINISHING THIS:
#CXXFLAGS_ADDITIONAL	+= -O0 -Wno-\#warnings
CXXFLAGS_ADDITIONAL	+= -gcolumn-info -g3 -fno-builtin
CXXFLAGS_ADDITIONAL += -fuse-ld=lld -Wno-unused-command-line-argument
# CXXFLAGS_ADDITIONAL	+= '-DERROR=((void)printf("ERROR HERE: " __FILE__ ":%d in %s\n", __LINE__, __func__), 1)'
#CXXFLAGS_ADDITIONAL	+= -fsanitize=address

export BUILD_DIR
export CXX
export CXXFLAGS_ADDITIONAL
export INCLUDE_DIR
export SRC_DIR


# All (make all)
all:
	@$(MAKE) --no-print-directory header
	@$(MAKE) --no-print-directory -f ./Webserv.mk $(PMAKE)
	@$(MAKE) --no-print-directory footer

bonus: 
	@$(MAKE) --no-print-directory header
	@$(MAKE) --no-print-directory -f ./Webserv.mk $(PMAKE) bonus
	@$(MAKE) --no-print-directory footer

#	Header
header:
	@echo -e ''
	@echo -e '$(GOLD)            *******     ****** ******* $(END)'
	@echo -e '$(GOLD)          ******        ***    ******* $(END)'
	@echo -e '$(GOLD)      *******           *      ******* $(END)'
	@echo -e '$(GOLD)     ******                  *******   $(END)'
	@echo -e '$(GOLD)  *******                  *******     $(END)'
	@echo -e '$(GOLD) *******************    *******      * $(END)'
	@echo -e '$(GOLD) *******************    *******    *** $(END)'
	@echo -e '$(GOLD)              ******    ******* ****** $(END)'
	@echo -e '$(GOLD)              ******                   $(END)'
	@echo -e '$(GOLD)              ******                   $(END)'
	@echo -e '$(GREY)           Made by maiboyerl x bgoulard$(END)'

#	Footer
footer:
	@echo -e '$(GOLD)$(END)'
	@echo -e '$(GOLD)   +------+                 +------+   $(END)'
	@echo -e '$(GOLD)  /|     /|                 |\     |\  $(END)'
	@echo -e '$(GOLD) +-+----+ |                 | +----+-+ $(END)'
	@echo -e '$(GOLD) | |    | |      $(CYAN)$(BOLD)$(UNDERLINE)cub3d$(END)$(GOLD)      | |    | | $(END)'
	@echo -e '$(GOLD) | +----+-+                 +-+----+ | $(END)'
	@echo -e '$(GOLD) |/     |/                   \|     \| $(END)'
	@echo -e '$(GOLD) +------+                     +------+ $(END)'
	@echo -e '$(GOLD)$(END)'
	@echo -e '            $(GREY)The compilation is $(END)$(GOLD)finished$(END)'
	@echo -e '                 $(GREY)Have a good $(END)$(GOLD)correction$(END)'

# Clean (make clean)
clean:
	@echo -e '$(GREY) Removing $(END)$(RED)Objects$(END)'
	@echo -e '$(GREY) Removing $(END)$(RED)Objects Folder$(END)'
	@$(RM) -r $(BUILD_DIR)

# Clean (make fclean)
fclean: clean
	@echo -e '$(GREY) Removing $(END)$(RED)Program$(END)'
	@$(RM) $(NAME)
	@$(RM) $(NAME)_bonus
	@echo ""

# Restart (make re)
re: 
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

tokei:
	@/bin/sh -c 'tokei'


filelist:
	@$(MAKE) --no-print-directory -f ./Webserv.mk filelist

.clangd:
	@rm -f .clangd
	@echo >> .clangd
	@echo 'CompileFlags: # Tweak the parse settings' >> .clangd
	@echo '  Compiler: clang' >> .clangd
	@echo '  Add:' >> .clangd
	@echo '    - "-xc++"' >> .clangd
	@echo '    - "-std=c++98"' >> .clangd
	@echo '    - "-I$(shell realpath $(INCLUDE_DIR))"' >> .clangd
	@echo >> .clangd


.clang-format:
	@rm -f .clang-format
	@curl https://raw.githubusercontent.com/Maix0/42cpp/refs/heads/master/.clang-format -o .clang-format

subject: subject.txt
	@bat --plain ./subject.txt

subject.txt:
	@curl $(SUBJECT_URL) | pdftotext -layout -nopgbrk -q - subject.txt

#	phony
.PHONY: all bonus clean fclean re header footer filelist .clangd .clang-format subject
