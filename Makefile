all: derby

derby: derby.c
	gcc derby.c -lpthread -o derby

clean: 
	rm derby
