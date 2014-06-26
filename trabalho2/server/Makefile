OBJ=auxf.o wrapsock.o state.o control.o
FLAGS=-Wall


build: $(OBJ)
	gcc $(FLAGS) -o server $(OBJ) server.c
	rm -f $(OBJ)

wrapsock.o : wrapsock.c
	gcc $(FLAGS) -c wrapsock.c
	
state.o : state.c
	gcc $(FLAGS) -c state.c
	
control.o : control.c
	gcc $(FLAGS) -c control.c
	
auxf.o : auxf.c
	gcc $(FLAGS) -c auxf.c

clean:
	rm -f $(OBJ) cliente servidor
