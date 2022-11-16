all: server
	gcc client.c -o client.bin -Wall -std=gnu99
server: lib
	gcc server.c -o server.bin -Wall -ldl -std=gnu99
lib:
	gcc lib.c -o libfiles.so -Wall -shared -fPIC -std=gnu99
clean:
	rm *.bin *.modified *.so
