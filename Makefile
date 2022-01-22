all: dwaf

dwaf: usage_example.o directory_traverser.o queue.o list.o get_opts_help.o 
	gcc -lm -pthread -g -std=gnu11 -Wall -o dwaf usage_example.o directory_traverser.o queue.o list.o get_opts_help.o 

usage_example.o: usage_example.c directory_traverser.h queue.h
	gcc -g -std=gnu11 -Wall -c usage_example.c

directory_traverser.o: directory_traverser.c directory_traverser.h queue.h get_opts_help.h
	gcc -g -std=gnu11 -Wall -c directory_traverser.c 

queue.o: queue.c queue.h list.h
	gcc -g -std=gnu11 -Wall -c queue.c

list.o: list.c list.h
	gcc -g -std=gnu11 -Wall -c list.c

get_opts_help.o: get_opts_help.c get_opts_help.h
	gcc -g -std=gnu11 -Wall -c get_opts_help.c

clean:
	rm  *.o dswaf
