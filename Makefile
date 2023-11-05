SRCS =	main.cpp \
		srcs/Webserv.cpp srcs/Configuration.cpp srcs/Server.cpp \
		# srcs/HTTPRequest.cpp srcs/HTTPResponse.cpp

OBJDIR = obj
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

NAME = webserv

CFLAGS = -Wall -Wextra -Werror -std=c++98

CXX = g++

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all
