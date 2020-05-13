jimmy_fallon: jimmy_fallon.o
	gcc jimmy_fallon.o -pthread -o jimmy_fallon

jimmy_fallon.o: jimmy_fallon.c
	gcc -c jimmy_fallon.c -o jimmy_fallon.o

clean:
	rm -f *.o jimmy_fallon
