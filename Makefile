SRCS =	main.cpp \
		srcs/Webserv.cpp srcs/Configuration.cpp srcs/Server.cpp srcs/HTTPRequest.cpp srcs/HTTPResponse.cpp

OBJS = $(SRCS:%cpp=%o)

NAME = webserv

CFLAGS = -Wall -Wextra -Werror -std=c++98

CC = c++

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)
