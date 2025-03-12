# **************************************************************************** #make
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rparodi <rparodi@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/12 11:05:05 by rparodi           #+#    #+#              #
#    Updated: 2025/03/12 15:43:53 by maiboyer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Objdir
BUILD_DIR		= $(shell realpath ./build)
SRC_DIR			=	./src
INCLUDE_DIR		=	./include

CXX=c++
MSG=
NAME=webserv

SUBJECT_URL						= https://cdn.intra.42.fr/pdf/pdf/150260/en.subject.pdf
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

WSTART = \033[D[$(RED)$(BOLD)$(UNDERLINE)WARNING$(END)]:$(CYAN) 
WEND = \n$(END)

LOG_DISABLE=
CXXFLAGS_ADDITIONAL= $(LOG_DISABLE)
PMAKE =
ifndef PMAKE_DISABLE
ifeq ($(shell uname), Linux)
    PMAKE = -j$(shell grep -c ^processor /proc/cpuinfo)
    #CFLAGS_ADDITIONAL    += -DPRINT_BACKTRACE
endif
ifeq ($(shell uname), Darwin)
    PMAKE = -j$(shell sysctl -n hw.ncpu)
    #CFLAGS_ADDITIONAL    += -DNVALGRIND
endif
endif

ifeq ($(MAKECMDGOALS), bonus)
    CFLAGS_ADDITIONAL += -DBONUS=1
    BUILD_DIR := $(BUILD_DIR)/bonus
endif

CXXFLAGS_ADDITIONAL	+= -gcolumn-info -g3 -fno-builtin
CXXFLAGS_ADDITIONAL += -fdiagnostics-color=always
CXXFLAGS_ADDITIONAL	+= -DLOG_LEVEL=debug

LLD := $(shell command -v lld 2> /dev/null)
ifdef LLD
    ifeq ($(MAKECMDGOALS), header)
        MSG += "$(WSTART)using $(GOLD)lld$(WEND)"
    endif
    CXXFLAGS_ADDITIONAL += -fuse-ld=lld -Wno-unused-command-line-argument
endif

#CXXFLAGS_ADDITIONAL	+= -fsanitize=address

ifdef MSG_BONUS
    MSG += "$(MSG_BONUS)"
endif

export BUILD_DIR
export CXX
export CXXFLAGS_ADDITIONAL
export INCLUDE_DIR
export SRC_DIR
export NAME

# All (make all)
all:
	$(eval CXXFLAGS_ADDITIONAL += -Werror)
	@$(MAKE) --no-print-directory header
	@$(MAKE) --no-print-directory -f ./Webserv.mk $(PMAKE)
	@$(MAKE) --no-print-directory footer

debug:
	@$(MAKE) --no-print-directory header 'MSG_BONUS=$(WSTART)$(RED)USING DEBUG BUILD !$(RED)$(WEND)'
	@$(MAKE) --no-print-directory -f ./Webserv.mk $(PMAKE)
	@$(MAKE) --no-print-directory footer

bonus: 
	$(eval CXXFLAGS_ADDITIONAL += -Werror -DBONUS=1)
	@$(MAKE) --no-print-directory header 'MSG_BONUS=\n\x1b[D$(GOLD)         compiling with bonus          $(WEND)\n'
	@$(MAKE) --no-print-directory -f ./Webserv.mk $(PMAKE) NAME=$(NAME)_bonus
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
	@echo -e '$(GREY)            Made by maiboyerlpb x bebou$(END)'
	@echo -en $(MSG);

PROJ = $(CYAN)$(BOLD)$(UNDERLINE)webserv$(END)$(GOLD)

#	Footer
footer:
	@echo -e '$(GOLD)                                       $(END)'
	@echo -e '$(GOLD)   _____                       _____   $(END)'
	@echo -e '$(GOLD)--'\''   __\_____           _____/__   `--$(END)'
	@echo -e '$(GOLD)         _____) $(PROJ) (_____         $(END)'
	@echo -e '$(GOLD)         __)               (__         $(END)'
	@echo -e '$(GOLD)        __)                 (__        $(END)'
	@echo -e '$(GOLD)--.______)                   (______.--$(END)'
	@echo -e '$(GOLD)                                       $(END)'
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


archive:
	@$(MAKE) --no-print-directory -f ./Webserv.mk archive 

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

subject: .subject.txt
	@bat --plain ./.subject.txt

.subject.txt:
	@curl $(SUBJECT_URL) | pdftotext -layout -nopgbrk -q - .subject.txt

#	phony
.PHONY: all bonus clean fclean re header footer filelist .clangd .clang-format subject archive
