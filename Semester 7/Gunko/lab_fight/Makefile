all: lib
	gcc main.c -o main.bin -Wall -ldl -std=gnu99
lib:
	gcc lib.c -o libcrc.so -Wall -shared -fPIC -std=gnu99
clean:
	rm *.bin *.so
