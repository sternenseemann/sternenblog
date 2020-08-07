# your template, must be a single .c file with the extension omitted
TEMPLATE = templates/simple

PREFIX = /usr/local
WEB_PATH = /share/sternenblog
MAN_PATH = /share/man
DOC_PATH = /share/doc/sternenblog

INSTALL = install
RM = rm
CP = cp

# convert from imagemagick
CONVERT = convert

CC = gcc
CFLAGS = -Wall -pedantic --std=c99 -Os

# debugging
# CFLAGS = -Wall -pedantic --std=c99 -ggdb -Og
