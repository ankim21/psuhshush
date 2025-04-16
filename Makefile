NAME = push_swap

CC = gcc

CFLAGS = -Wall -Wextra -Werror -g3

SRCS :=    algo1.c \
			algo2.c \
			algo3.c \
			parsing1.c \
			parsing2.c \
			parsing3.c \
			push_swap.c \
			rotate1.c \
			rotate.c \
			swap.c \
			push.c \
			pushalgo.c \

LIB_PATH=LIBFT
LIB_SRC=LIBFT/libft.a

PRINTF_PATH=PRINTF
PRINTF_SRC=PRINTF/libftprintf.a

OBJ = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJ) $(LIB_SRC) $(PRINTF_SRC)
	$(CC) $(CFLAGS) -no-pie $(OBJ)  $(LIB_SRC) $(PRINTF_SRC) -o $(NAME)

debug: 
		$(CC) $(CFLAGS) -no-pie -g $(SRCS)  $(LIB_SRC) $(PRINTF_SRC) -o $(NAME)

$(LIB_SRC):
	$(MAKE) -C $(LIB_PATH) 
$(PRINTF_SRC):
	$(MAKE) -C $(PRINTF_PATH)
clean:
	rm -rf $(OBJ)
	$(MAKE) -C $(LIB_PATH) clean
	$(MAKE) -C $(PRINTF_PATH) clean
fclean: clean
	rm -rf $(NAME)
	$(MAKE) -C $(LIB_PATH) fclean
	$(MAKE) -C $(PRINTF_PATH) fclean
re: fclean all

.PHONY: all clean fclean re