# **************************************************************************** #make
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rparodi <rparodi@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/12 11:05:05 by rparodi           #+#    #+#              #
#    Updated: 2025/05/10 14:11:35 by maiboyer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Objdir
BUILD_DIR		= $(shell realpath ./build)
SRC_DIR			=	./src
INCLUDE_DIR		=	./include
DOWNLOAD_DIR	=	./dl

CXX=c++
MSG=
NAME=webserv

SUBJECT_URL						= https://cdn.intra.42.fr/pdf/pdf/150260/en.subject.pdf
SUBJECT_URL_UBUNTU_CGI_TESTER	= https://cdn.intra.42.fr/document/document/31582/ubuntu_cgi_tester
SUBJECT_URL_UBUNTU_TESTER		= https://cdn.intra.42.fr/document/document/31584/ubuntu_tester

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

WSTART = \033[D[$(RED)$(BOLD)$(UNDERLINE)WARNING$(END)]$(CYAN) 
WEND = \n$(END)

LOG_DISABLE=
CXXFLAGS_ADDITIONAL= $(LOG_DISABLE)
LDFLAGS_ADDITIONAL=
PMAKE =
ifndef PMAKE_DISABLE
ifeq ($(shell uname), Linux)
    PMAKE = -j$(shell grep -c ^processor /proc/cpuinfo)
endif
ifeq ($(shell uname), Darwin)
    PMAKE = -j$(shell sysctl -n hw.ncpu)
endif
endif

ifeq ($(MAKECMDGOALS), bonus)
    CXXFLAGS_ADDITIONAL += -DBONUS=1
    BUILD_DIR := $(BUILD_DIR)/bonus
endif

ifeq ($(CXX), g++)
    CXXFLAGS_ADDITIONAL += -Wno-c++11-compat -Wno-type-limits
endif


ENABLE_BACKTRACE ?= no
ifeq ($(ENABLE_BACKTRACE), yes)
    LDFLAGS_ADDITIONAL += -rdynamic
    CXXFLAGS_ADDITIONAL += -DTERMINATE_BACKTRACE
    ifeq ($(MAKECMDGOALS), header)
        MSG += "$(WSTART)using $(GOLD)custom terminate func$(WEND)"
    endif
endif

ENABLE_SOCKET_PORT ?= no
ifeq ($(ENABLE_BACKTRACE), yes)
    CXXFLAGS_ADDITIONAL += -DENABLE_SOCKET_PORT
    ifeq ($(MAKECMDGOALS), header)
        MSG += "$(WSTART)printing last socket port onto /tmp/socket_webserv$(WEND)"
    endif
endif

ENABLE_PRINT_PID ?= yes
ifeq ($(ENABLE_PRINT_PID), yes)
    CXXFLAGS_ADDITIONAL += -DENABLE_PRINT_PID
    ifeq ($(MAKECMDGOALS), header)
        MSG += "$(WSTART)using $(GOLD)priting pids$(WEND)"
    endif
endif

#CXXFLAGS_ADDITIONAL	+= -gcolumn-info -g3 -fno-builtin
#CXXFLAGS_ADDITIONAL	+= -fdiagnostics-color=always
#CXXFLAGS_ADDITIONAL	+= -DLOG_LEVEL=debug

ENABLE_LLD ?= no
LLD := $(shell command -v lld 2> /dev/null)
ifdef LLD
    ifeq ($(ENABLE_LLD),yes)
        ifeq ($(MAKECMDGOALS), header)
            MSG += "$(WSTART)using $(GOLD)lld$(WEND)"
        endif
        LDFLAGS_ADDITIONAL += -fuse-ld=lld
    endif
endif

#CXXFLAGS_ADDITIONAL	+= -fsanitize=address

ifdef MSG_BONUS
    MSG += "$(MSG_BONUS)"
endif


SCAN_BUILD := $(shell command -v scan-build 2> /dev/null)
ifndef SCAN_BUILD
	SCAN_BUILD := $(shell command -v scan-build-12 2> /dev/null)
endif

BASE_PATH=$(shell realpath .)

export BUILD_DIR
export CXX
export CXXFLAGS_ADDITIONAL
export LDFLAGS_ADDITIONAL
export INCLUDE_DIR
export SRC_DIR
export NAME
export BASE_PATH

ECHO = /usr/bin/env echo
export ECHO

all:
	$(eval CXXFLAGS_ADDITIONAL += -Werror)
	@$(MAKE) --no-print-directory header
	@$(MAKE) --no-print-directory -f ./Webserv.mk $(PMAKE)
	@$(MAKE) --no-print-directory footer
$(NAME): all;

scan-build:
	@$(SCAN_BUILD) $(MAKE) --no-print-directory -k re                  \
		CXXFLAGS_ADDITIONAL=-DLOG_DISABLE ENABLE_BACKTRACE=no ENABLE_LLD=no    \
		ENABLE_PRINT_PID=no                   \
		"MSG_BONUS=$(WSTART)$(RED)SCAN BUILD IS RUNNING$(WEND)"

release:
	$(eval CXXFLAGS_ADDITIONAL += -Werror)
	$(eval CXXFLAGS_ADDITIONAL += -O2)
	$(eval CXXFLAGS_ADDITIONAL += -march=native)
	$(eval CXXFLAGS_ADDITIONAL += -mtune=native)
	@$(MAKE) --no-print-directory header 'MSG_BONUS=$(WSTART)$(RED)Realese build - Added flags !$(RED)$(WEND)'
	@$(MAKE) --no-print-directory -f ./Webserv.mk $(PMAKE)
	@$(ECHO) -e '\033[90m Stripping\t\033[32m $(NAME)\033[0m'
	@/usr/bin/env strip -s $(NAME)
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

asan: 
	$(eval CXXFLAGS_ADDITIONAL = "")
	$(eval CXXFLAGS_ADDITIONAL = -fsanitize=address -fno-omit-frame-pointer)
	@$(MAKE) --no-print-directory header 'MSG_BONUS=\n\x1b[D$(GOLD)         ASAN          $(WEND)\n'
	@$(MAKE) --no-print-directory all $(PMAKE)
	@$(MAKE) --no-print-directory footer

#	Header
header:
	@$(ECHO) -e ''
	@$(ECHO) -e '$(GOLD)            *******     ****** ******* $(END)'
	@$(ECHO) -e '$(GOLD)          ******        ***    ******* $(END)'
	@$(ECHO) -e '$(GOLD)      *******           *      ******* $(END)'
	@$(ECHO) -e '$(GOLD)     ******                  *******   $(END)'
	@$(ECHO) -e '$(GOLD)  *******                  *******     $(END)'
	@$(ECHO) -e '$(GOLD) *******************    *******      * $(END)'
	@$(ECHO) -e '$(GOLD) *******************    *******    *** $(END)'
	@$(ECHO) -e '$(GOLD)              ******    ******* ****** $(END)'
	@$(ECHO) -e '$(GOLD)              ******                   $(END)'
	@$(ECHO) -e '$(GOLD)              ******                   $(END)'
	@$(ECHO) -e '$(GREY)            Made by maiboyerlpb x bebou$(END)'
	@$(ECHO) -e -n $(MSG);

PROJ = $(CYAN)$(BOLD)$(UNDERLINE)webserv$(END)$(GOLD)

#	Footer
footer:
	@$(ECHO) -e '$(GOLD)                                       $(END)'
	@$(ECHO) -e '$(GOLD)   _____                       _____   $(END)'
	@$(ECHO) -e '$(GOLD)--'\''   __\_____           _____/__   `--$(END)'
	@$(ECHO) -e '$(GOLD)         _____) $(PROJ) (_____         $(END)'
	@$(ECHO) -e '$(GOLD)         __)               (__         $(END)'
	@$(ECHO) -e '$(GOLD)        __)                 (__        $(END)'
	@$(ECHO) -e '$(GOLD)--.______)                   (______.--$(END)'
	@$(ECHO) -e '$(GOLD)                                       $(END)'
	@$(ECHO) -e '            $(GREY)The compilation is $(END)$(GOLD)finished$(END)'
	@$(ECHO) -e '                 $(GREY)Have a good $(END)$(GOLD)correction$(END)'

# Clean (make clean)
clean:
	@$(ECHO) -e '$(GREY) Removing $(END)$(RED)Objects$(END)'
	@$(ECHO) -e '$(GREY) Removing $(END)$(RED)Objects Folder$(END)'
	@$(RM) -r $(BUILD_DIR)

# Clean (make fclean)
fclean: clean
	@$(ECHO) -e '$(GREY) Removing $(END)$(RED)Program$(END)'
	@$(RM) $(NAME)
	@$(RM) $(NAME)_bonus
	@$(ECHO) ""

# Restart (make re)
re: 
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

tokei:
	@/bin/sh -c 'tokei'


filelist:
	@$(MAKE) --no-print-directory -f ./Webserv.mk filelist


archive:
	$(eval CXXFLAGS_ADDITIONAL += -Werror)
	@$(MAKE) --no-print-directory -f ./Webserv.mk archive $(PMAKE)

.clangd:
	@rm -f .clangd
	@$(ECHO) >> .clangd
	@$(ECHO) 'CompileFlags: # Tweak the parse settings' >> .clangd
	@$(ECHO) '  Compiler: clang' >> .clangd
	@$(ECHO) '  Add:' >> .clangd
	@$(ECHO) '    - "-xc++"' >> .clangd
	@$(ECHO) '    - "-std=c++98"' >> .clangd
	@$(ECHO) '    - "-I$(shell realpath $(INCLUDE_DIR))"' >> .clangd
	@$(ECHO) >> .clangd

#-fsanitize=address -fno-omit-frame-pointer
tests:
	@$(CXX) -Wall -Wextra -DLOG_LEVEL=trace -U_FORTIFY_SOURCE -std=c++98 -no-pie \
			-ggdb3 -O0 -fdiagnostics-color \
			-I$(INCLUDE_DIR) \
			$(SRC_DIR)/runtime/shim.cpp $(SRC_DIR)/runtime/Logger.cpp $(SRC_DIR)/lib/Semaphore.cpp \
			./test/Rc.cpp \
			-o ./tests

subject: .subject.txt
	@bat --plain ./.subject.txt

.subject.txt:
	@curl $(SUBJECT_URL) | pdftotext -layout -nopgbrk -q - .subject.txt

cleandl:
	rm -rf $(DOWNLOAD_DIR)

download:  $(DOWNLOAD_DIR)/cgi_tester $(DOWNLOAD_DIR)/tester 
	chmod +x $^

$(DOWNLOAD_DIR)/cgi_tester: $(DOWNLOAD_DIR)
	curl $(SUBJECT_URL_UBUNTU_CGI_TESTER) -o $@
	
$(DOWNLOAD_DIR)/tester: $(DOWNLOAD_DIR)
	curl $(SUBJECT_URL_UBUNTU_TESTER) -o $@

$(DOWNLOAD_DIR):
	@mkdir -p $(DOWNLOAD_DIR)

SYMDIFF_CLASSES = 
# this can be overriten to add flags (-D to demangle, -A to unconditionnally add all classes)
SYMDIFF_FLAGS = 
symdiff:
	@$(MAKE) --no-print-directory -f ./Webserv.mk symdiff $(PMAKE)

#	phony
.PHONY: all bonus clean fclean re header footer filelist .clangd .clang-format subject archive tests
