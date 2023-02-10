all: a.out b.out

b.out: node.o kmp.o
	gcc node.o kmp.o -o b.out -lzmq

a.out: main.o
	gcc main.o -o a.out -lzmq

node.o: node.c
	gcc -c node.c node.o

main.o: main.c
	gcc -c main.c main.o

kmp.o: kmp.c
	gcc -c kmp.c kmp.o

clean:
	rm -f *.o
	rm -f *.out

clean_no_out:
	rm -f *.o
