NAME		= PmergeMe

SRC			=	main.cpp PmergeMe.cpp\

OBJS		= $(SRC:.cpp=.o)

CXX			= c++

CXXFLAGS		= -Wall -Werror -Wextra -std=c++98 -pedantic -g

RM			= rm -rf

all: $(NAME) 

$(NAME): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean:	clean
	$(RM) $(NAME)

re:	fclean all

.PHONY:	all clean fclean re
