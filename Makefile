# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: daong <daong@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/15 09:42:59 by daong             #+#    #+#              #
#    Updated: 2025/03/15 09:43:08 by daong            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

DIR			= .
DIR_SERVER_CONFIG = ./serverConfig
DIR_SERVER  = ./server
DIR_REQ  = ./request
DIR_RES  = ./response

SRC	= ${wildcard ${DIR_SERVER}/*.cpp} \
			${wildcard ${DIR_REQ}/*.cpp} \
			${wildcard ${DIR_RES}/*.cpp} \
			${wildcard ${DIR}/*.cpp} \
			${wildcard ${DIR_SERVER_CONFIG}/*.cpp} \
			# ${DIR}/main.cpp \
			# ${DIR_SERVER_CONFIG}/WebServerConfig.cpp \
			# ${DIR_SERVER_CONFIG}/ConfigGlobal.cpp \
			# ${DIR_SERVER_CONFIG}/ConfigLocation.cpp \
			# ${DIR_SERVER_CONFIG}/ConfigServer.cpp \
			# ${DIR_SERVER_CONFIG}/utilConfig.cpp \
			# ${DIR_SERVER}/GlobalServer.cpp

HEADER= ${wildcard ${INCLUDE}/*.hpp} \
				# ${INCLUDE}/WebServerConfig.hpp \
			  # ${INCLUDE}/ConfigGlobal.hpp \
			  # ${INCLUDE}/ConfigLocation.hpp \
			  # ${INCLUDE}/ConfigServer.hpp \
			  # ${INCLUDE}/utilConfig.hpp \
			  # ${INCLUDE}/utilConfig.hpp \
			  # ${INCLUDE}/GlobalServer.hpp

OBJS		= ${SRC:.cpp=.o}


INCLUDE		= ./header

NAME	    = webserv

AR			= ar rcs

CPP			= c++ -Wall -Wextra -Werror

STD98_FLAG  = -std=c++98

all: ${NAME}

%.o: %.cpp ${HEADER}
			${CPP} ${STD98_FLAG} -c -g -I ${INCLUDE}  $< -o ${<:.cpp=.o}

${NAME}: ${OBJS}
			${CPP}  -g -o ${NAME} ${OBJS}

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean all

.PHONY: all clean fclean re

run: ${NAME}
	./${NAME}

v : ${NAME}
	valgrind ./${NAME}