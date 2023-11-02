SRCS =	main.cpp \
		srcs/Webserv.cpp \
		srcs/config/uitlParsing.cpp srcs/config/check_utils.cpp srcs/config/Config.cpp \
		srcs/server/Server.cpp \
		srcs/server/location/Location.cpp \

# SRCS =	main.cpp \
# 		srcs/Webserv.cpp srcs/config/Configuration.cpp srcs/Server.cpp \
		# srcs/HTTPRequest.cpp srcs/HTTPResponse.cpp

OBJDIR = obj
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

NAME = webserv

CFLAGS = -Wall -Wextra -Werror -std=c++98

CC = c++

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
