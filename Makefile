all: derby derby2

derby: derby.c
	gcc derby.c -lpthread -o derby

derby2: derby2.c
	gcc derby2.c -lpthread -o derby2

clean: 
	rm derby derby2
