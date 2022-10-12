all: lib
	gcc -Wall main.c -o main.bin -ldl
lib:
	gcc -fPIC -Wall lib.c -shared -o libfiles.so
clean:
	rm *.bin *.modified *.so