NAME    = codexion

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread -I.

SRCS    = src/main.c \
          src/init.c \
          src/utils.c \
          src/routine.c \
          src/routine_helper.c \
          src/monitor.c \
          src/queue.c

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