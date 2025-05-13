NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS = main.cpp \
	   src/Parser.cpp \
	   src/CheckConfig.cpp \
	   src/LocationConf.cpp \
	   src/ServerConf.cpp \
	   src/Tokenizer.cpp \
	   src/WebServer.cpp \
	   src/HelperClass.cpp \
	   src/HttpRequest.cpp \
	   src/MethodTransaction.cpp \
	   src/FileHandler.cpp 

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
