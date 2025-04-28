NAME_CLIENT = client
NAME_SERVER = server

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC_CLIENT = client.c
SRC_SERVER = server.c

OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)

all: $(NAME_CLIENT) $(NAME_SERVER)

$(NAME_CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $(NAME_CLIENT) $(OBJ_CLIENT)
	@echo "Client compiled successfully!"

$(NAME_SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -o $(NAME_SERVER) $(OBJ_SERVER)
	@echo "Server compiled successfully!"

clean:
	rm -f $(OBJ_CLIENT) $(OBJ_SERVER)
	@echo "Object files removed!"

fclean: clean
	rm -f $(NAME_CLIENT) $(NAME_SERVER)
	@echo "Executables removed!"

re: fclean all

.PHONY: all clean fclean re