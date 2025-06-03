NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIRS = . src
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

OBJDIR = obj
OBJNAMES = $(notdir $(SRCS:.cpp=.o))
OBJS = $(addprefix $(OBJDIR)/, $(OBJNAMES))

vpath %.cpp $(SRC_DIRS)

all: 
	@echo "\033[1;36m⚡ Compiling WebServ...\033[0m"
	@$(MAKE) $(NAME) --no-print-directory

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@echo "\033[33m"
	@echo " \\ \\        / / | |   / ____|                "
	@echo "  \\ \\  /\\  / /__| |__| (___   ___  _ __   __"
	@echo "\033[34m   \\ \\/  \\/ / * \\ '* \\\\___ \\ / * \\ '*_\\ \\ / /"
	@echo "    \\  /\\  /  __/ |_) |___) |  __/ |   \\ V / "
	@echo "     \\/  \\/ \\___|_.__/_____/ \\___|_|    \\_/  "
	@echo "                                             \033[0m"
	@echo "\033[1;32m WebServ is ready to serve!\033[0m"

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)
	@echo "\033[4;33mObject files removed!\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[1;31m WebServ executable deleted!\033[0m"

re: fclean all

.PHONY: all clean fclean re