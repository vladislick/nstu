all: child
	gcc parent.c -o parent.bin -Wall -std=gnu99
child:
	gcc main.c -o main.bin -Wall -std=gnu99
clean:
	rm *.bin *.modified
