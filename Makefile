NAME			=	ircserv
CC				=	c++
CFLAGS			=	-Wall -Werror -Wextra -Iincludes
STD				=	-std=c++98

RM				=	rm -rf

SRCS			=	srcs/main.cpp \
					srcs/Client.cpp \
					srcs/Server.cpp \
					srcs/Bot.cpp \
					srcs/Parser.cpp \
					srcs/Commands.cpp \
					srcs/CmdUtils.cpp \
					srcs/cmds/Helpers.cpp \
					srcs/cmds/Invite.cpp \
					srcs/cmds/Join.cpp \
					srcs/cmds/Kick.cpp \
					srcs/cmds/Mode.cpp \
					srcs/cmds/Nick.cpp \
					srcs/cmds/Part.cpp \
					srcs/cmds/Pass.cpp \
					srcs/cmds/Privmsg.cpp \
					srcs/cmds/Quit.cpp \
					srcs/cmds/Topic.cpp \
					srcs/cmds/User.cpp

HEADERS			=	includes/Channel.hpp \
					includes/Client.hpp \
					includes/Command.hpp \
					includes/Helpers.hpp \
					includes/Parser.hpp \
					includes/Server.hpp \
					includes/Bot.hpp

OBJ_DIR			=	obj
OBJ				=	$(SRCS:srcs/%.cpp=$(OBJ_DIR)/%.o)

COLOR_YELLOW	=	\033[0;33m
COLOR_GREEN		=	\033[0;32m
COLOR_RED		=	\033[0;31m
COLOR_END		=	\033[0m

all				:	$(NAME)

$(NAME)			:	$(OBJ)
					@$(CC) $(CFLAGS) $(STD) $(OBJ) -o $(NAME)
					@echo "\n\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_GREEN)[is ready!]\e[0m\n$(COLOR_END)"

$(OBJ_DIR)/%.o	:	srcs/%.cpp $(HEADERS) | $(OBJ_DIR)
					@mkdir -p $(dir $@)
					@$(CC) $(CFLAGS) $(STD) -c $< -o $@

$(OBJ_DIR)		:
					@mkdir -p $(OBJ_DIR)

clean			:
					@$(RM) -rf $(OBJ_DIR)
					@echo "\e[1m$(COLOR_YELLOW)objects		$(COLOR_RED)[are deleted!]\e[0m$(COLOR_END)"

fclean			:	clean
					@$(RM) $(NAME)
					@echo "\e[1m$(COLOR_YELLOW)$(NAME)	$(COLOR_RED)[is deleted!]\e[0m$(COLOR_END)"

re				:	fclean all

.PHONY			:	all clean fclean re