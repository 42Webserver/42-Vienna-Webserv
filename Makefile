NAME=	webserv
FLAGS=	-Wall -Werror -Wextra -pedantic -std=c++98 -g

FILES=	main.cpp Webserver.cpp Server.cpp Connection.cpp Request.cpp Response.cpp PollContainer.cpp ConfigParser.cpp Data.cpp CGI.cpp
OBJS=	$(FILES:.cpp=.o)

all:	$(NAME)

%.o:		%.cpp
	c++ $(FLAGS) -o $@ -c $<

$(NAME):	$(OBJS)
	c++ $(FLAGS) $(OBJS) -o $(NAME)

debug:
	make FLAGS="$(FLAGS) -D DEBUG"

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY:	clean fclean re all
