all: child
	gcc parent.c -o parent.bin -Wall -std=gnu99
child: lib
	gcc -Wall main.c -o main.bin -ldl -std=gnu99
lib:
	gcc -fPIC -Wall lib.c -shared -o libfiles.so -std=gnu99
clean:
	rm *.bin *.modified *.so
