# makefile for slurp

CC=g++
SRCDIR=src
OBJDIR=obj
INCDIR=inc
BINDIR=bin
CFLAGS=-c -Wall -I$(INCDIR)
LDFLAGS=
MODULES=slurper retriever tokenizer parser eventer
OBJECTS=$(MODULES:=.o)
EXECUTABLE=slurp

all: $(MODULES) $(EXECUTABLE)

$(MODULES):
	$(CC) $(CFLAGS) $(SRCDIR)/$@.cpp -o $(OBJDIR)/$@.o 

$(EXECUTABLE):
	$(CC) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OBJECTS)) -o $(BINDIR)/$@

clean: 
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(EXECUTABLE)
	

