NAME=	webserv
FLAGS=	-Wall -Werror -Wextra -pedantic -std=c++98 -g

SRC_DIR= ./src/
OBJ_DIR= ${SRC_DIR}objs/

FILES=	main.cpp Webserver.cpp Server.cpp Connection.cpp Request.cpp Response.cpp PollContainer.cpp ConfigParser.cpp Data.cpp CGI.cpp Timer.cpp FilePath.cpp

SRCS=	$(addprefix $(SRC_DIR), $(FILES))
OBJS=	$(addprefix $(OBJ_DIR), $(FILES:.cpp=.o))

all:	$(NAME)

$(OBJ_DIR)%.o:		$(SRC_DIR)%.cpp
	c++ $(FLAGS) -o $@ -c $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(NAME):	$(OBJ_DIR) $(OBJS)
	c++ $(FLAGS) $(OBJS) -o $(NAME)

debug:
	make FLAGS="$(FLAGS) -D DEBUG"

run: $(NAME)
	envsubst < webserv.conf > webserv_run.conf
	./webserv webserv_run.conf

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY:	clean fclean re all
