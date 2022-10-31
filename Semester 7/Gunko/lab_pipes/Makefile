all: server
	gcc client.c -o client.bin -Wall -std=gnu99
server: lib
	gcc server.c -o server.bin -Wall -std=gnu99 -ldl
lib:
	gcc lib.c -shared -o libfiles.so -fPIC -Wall -std=gnu99
clean:
	rm *.bin *.modified *.so
