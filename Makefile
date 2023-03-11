# Author: Peng Li
# Student ID: 3012769
# MacEwan University CMPT 399: Compilers Fall 2020
SHELL=/bin/bash

CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS=-c
.PHONY: all clean

all: mgo
mgo: lexer.o lexer.h parser.o tree.o analyze.o symtab.o codegen.o mgo.c
	${CC} ${CFLAGS} $^  -o $@

lexer.o: lexer.c parser.h
	${CC} ${CFLAGS} ${LDFLAGS} $<  -o $@

lexer.h lexer.c: lexer.l
	flex --header-file=lexer.h -o lexer.c lexer.l

tree.o: tree.c tree.h
	$(CC) ${CFLAGS} ${LDFLAGS} -c -o $@ $<

symtab.o: symtab.c symtab.h
	$(CC) ${CFLAGS} ${LDFLAGS} -o $@ $<

analyze.o: analyze.c
	$(CC) ${CFLAGS} ${LDFLAGS} -o $@ $^

parser.o: parser.h parser.c
	$(CC) ${CFLAGS} ${LDFLAGS} parser.c -o $@

io.o: io.c
	# CFLAGS is omitted intentionally due to warning produced at line 40 of io.c
	$(CC) ${LDFLAGS} -m32 $< -o $@

codegen.o: codegen.c codegen.h
	$(CC) ${CFLAGS} ${LDFLAGS} $< -o $@

parser.h parser.c: lemon
	./lemon parser.y

lemon: lemon.c
# -pedantic flags is emitted for lemon to suppress warning
	$(CC) -Wall -std=c99 -o $@ $^

clean:
	@rm -f {lexer,parser}.{o,h,c}  {tree,analyze,symtab,codegen,io}.o parser.out lemon mgo

