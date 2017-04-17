all:
	gcc crossing-threads.c animation.c -o exec -lm -pthread
clean:
	rm exec
