DIR			= .
DIR_SERVER_CONFIG = ./serverConfig
DIR_SERVER  = ./server

SRC			= ${DIR}/main.cpp \
			  ${DIR_SERVER_CONFIG}/WebServerConfig.cpp \
			  ${DIR_SERVER_CONFIG}/ConfigGlobal.cpp \
			  ${DIR_SERVER_CONFIG}/ConfigLocation.cpp \
			  ${DIR_SERVER_CONFIG}/ConfigServer.cpp \
			  ${DIR_SERVER_CONFIG}/utilConfig.cpp \
			  ${DIR_SERVER}/GlobalServer.cpp

HEADER      = ${INCLUDE}/WebServerConfig.hpp \
			  ${INCLUDE}/ConfigGlobal.hpp \
			  ${INCLUDE}/ConfigLocation.hpp \
			  ${INCLUDE}/ConfigServer.hpp \
			  ${INCLUDE}/utilConfig.hpp \
			  ${INCLUDE}/GlobalServer.hpp

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
