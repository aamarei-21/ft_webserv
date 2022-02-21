NAME		= webserv

CFLAGS		= -Wall -Wextra -Werror -std=c++98
CC			= clang++

INCLUDES	= ./includes/

SRCDIR		= ./src/

FILES		= \
			main.cpp \
			AHttpMsg.cpp \
			Request.cpp \
			utils.cpp \
			pages.cpp \
			MD5.cpp
# Tests.cpp \


SRCS 		= $(addprefix $(SRCDIR), $(FILES))

all: $(NAME)

$(NAME):		$(INCLUDES)
	@echo '----Making webserver ------'
	$(CC)		$(CFLAGS) $(SRCS) -I$(INCLUDES) -o $(NAME)
	@echo "Ready"


clean:
	rm -rf		$(OBJDIR) *.o *.d

fclean:		clean
	rm -f		$(NAME)
	rm -rf		.*.tmp *.tmp
	rm -rf		./www/root/localhost/uploads/*

re:		fclean all

norma:
	norminette $(SRCDIR) ./includes/

bonus:	all

.PHONY: all  clean fclean re bonus
