NAME		=	philosopher
CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror
SRCS		=	test.c				\

OBJS		=	$(SRCS:%.c=%.o)

all:			$(NAME)

