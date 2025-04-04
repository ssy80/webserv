# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: daong <daong@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/15 09:42:59 by daong             #+#    #+#              #
#    Updated: 2025/03/22 13:53:57 by daong            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

DIR			= .
DIR_SERVER_CONFIG = ./serverConfig
DIR_SERVER  = ./server
DIR_REQ  = ./request
DIR_RES  = ./response

SRC	=	${DIR}/main.cpp \
		${DIR_REQ}/Request.cpp \
		${DIR_REQ}/RequestParser.cpp \
		${DIR_RES}/ResHelper.cpp \
		${DIR_RES}/Response.cpp \
		${DIR_SERVER}/AServer.cpp \
		${DIR_SERVER}/GlobalServer.cpp \
		${DIR_SERVER}/MIME.cpp \
		${DIR_SERVER_CONFIG}/ConfigGlobal.cpp \
		${DIR_SERVER_CONFIG}/ConfigLocation.cpp  \
		${DIR_SERVER_CONFIG}/ConfigServer.cpp \
		${DIR_SERVER_CONFIG}/utilConfig.cpp \
		${DIR_SERVER_CONFIG}/WebServerConfig.cpp

HEADER= ${INCLUDE}/AServer.hpp \
		${INCLUDE}/ConfigGlobal.hpp \
		${INCLUDE}/ConfigLocation.hpp \
		${INCLUDE}/ConfigServer.hpp \
		${INCLUDE}/GlobalServer.hpp \
		${INCLUDE}/MIME.hpp \
		${INCLUDE}/Request.hpp \
		${INCLUDE}/RequestParser.hpp \
		${INCLUDE}/ResHelper.hpp \
		${INCLUDE}/Response.hpp \
		${INCLUDE}/utilConfig.hpp \
		${INCLUDE}/WebServerConfig.hpp \

OBJS		= ${SRC:.cpp=.o}


INCLUDE		= ./header

NAME	    = webserv

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

upload:
	curl -F 'file=@./Makefile' http://localhost:8080/upload/ -i

wrong:
	curl -X POST -H "Content-Type: multipart/form-data" http://localhost:8080/upload/ -i
