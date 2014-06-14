CC=gcc
CPP=g++
LEX=flex
YACC=bison
LD=gcc

all:	compilator
	cp compilator tests/compilator

compilator:	def.tab.o lex.yy.o Nodes.o
	$(CPP) lex.yy.o def.tab.o Nodes.o -o compilator -ll -ggdb -O0 -g

lex.yy.o:	lex.yy.c
	$(CC) -c lex.yy.c -ggdb -O0 -g

lex.yy.c: lex.ll
	$(LEX) lex.ll

def.tab.o:	def.tab.cc Nodes.cpp
	$(CPP) -c def.tab.cc Nodes.cpp -ggdb -O0 -g

def.tab.cc:	def.yy
	$(YACC) -d def.yy

clean:
	rm *.o compilator *.cc *.hh
