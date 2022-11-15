all: lib
	gcc main.c -o main.bin -Wall -lpthread -std=gnu99
lib:
	gcc -fPIC -Wall lib.c -shared -o libfiles.so -std=gnu99
clean:
	rm *.bin *.modified *.so
