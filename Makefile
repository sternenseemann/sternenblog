CC = gcc
TEMPLATE = templates/simple.c
CFLAGS = -Wall --debug --std=c99

default: blog.cgi

clean:
	rm -f blog.cgi

%.cgi: main.c $(TEMPLATE)
	$(CC) -o $@ $? $(CFLAGS)
