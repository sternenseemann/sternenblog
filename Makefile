CC = gcc
TEMPLATE = templates/simple.c
CFLAGS = -Wall --debug --std=c99

blog:
	$(CC) -o blog main.c $(TEMPLATE) $(CFLAGS)
clean:
	rm -f blog

again: clean blog
