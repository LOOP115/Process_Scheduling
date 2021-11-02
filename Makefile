allocate: allocate.o scheduler.o tool.o list.o
	gcc -Wall -o allocate allocate.o scheduler.o tool.o list.o -g -lm

scheduler.o: scheduler.c scheduler.h
	gcc -Wall -c -o scheduler.o scheduler.c -g -lm

tool.o: tool.c tool.h
	gcc -Wall -c -o tool.o tool.c -g
	
list.o: list.c list.h
	gcc -Wall -c -o list.o list.c -g

allocate.o: allocate.c
	gcc -Wall -c -o allocate.o allocate.c -g

clean:
	rm -f  allocate allocate.o scheduler.o tool.o list.o
