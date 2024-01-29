SRCS =	main.cpp \
		srcs/Webserv.cpp srcs/Configuration.cpp srcs/Server.cpp srcs/CGI.cpp \
		# srcs/HTTPRequest.cpp srcs/HTTPResponse.cpp

OBJDIR = obj
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

NAME = webserv

# CFLAGS = -Wall -Wextra -Werror -std=c++98

CC = g++

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all
