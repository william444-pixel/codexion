NAME    = codexion

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread -I.

SRCS    = main.c \
          init.c \
          utils.c \
          routine.c \
          routine_helper.c \
          monitor.c \
          queue.c

OBJS    = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re