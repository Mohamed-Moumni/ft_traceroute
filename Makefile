NAME = ft_traceroute

CC = gcc

FLAGS = 

SRC_FOLDER = src

OBJ_FOLDER = src

SRC = src/main.c src/utils.c src/ft_traceroute.c

HEADER = src/ft_traceroute.h

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME):$(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) -lm 

$(OBJ_FOLDER)/%.o:$(OBJ_FOLDER)/%.c $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean:
	rm -f $(OBJ) $(NAME)

re: fclean  all