all: lib
	gcc -c main.c -Wall -std=c99
	gcc -o main.bin main.o libfiles.a
	rm main.o
lib:
	gcc -c lib.c -Wall -std=c99
	ar cr libfiles.a lib.o
	rm lib.o
	ranlib libfiles.a
clean:
	rm *.bin *.modified *.a