OBJ=auxf.o wrapsock.o
FLAGS=-Wall


build: $(OBJ)
	gcc $(FLAGS) -o servidor $(OBJ) serveco.c
	gcc $(FLAGS) -o cliente $(OBJ) cliente.c
	rm -f $(OBJ)

auxf.o : auxf.c
	gcc $(FLAGS) -c auxf.c

wrapsock.o : wrapsock.c
	gcc $(FLAGS) -c wrapsock.c

clean:
	rm -f $(OBJ) cliente servidor
