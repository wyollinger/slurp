# executables
CC=g++
FLEX=flex

# files
MODULES=slurper retriever scanner eventer
OBJECTS=$(MODULES:=.o)
FLEXFILE=htmlscanner
EXECUTABLE=slurp

#directories
FLEXDIR=flex
SRCDIR=src
OBJDIR=obj
INCDIR=inc
BINDIR=bin

#flags
CFLAGS=-c -g -Wall -I$(INCDIR)
FLEXFLAGS=--read --full --header-file=$(INCDIR)/$(FLEXFILE).h --outfile=$(SRCDIR)/$(FLEXFILE).c
LDFLAGS=

#rules
all: $(FLEXFILE) $(MODULES) $(EXECUTABLE)

$(FLEXFILE):
	$(FLEX) $(FLEXFLAGS) $(FLEXDIR)/$(FLEXFILE).flex 
	$(CC) $(CFLAGS) $(SRCDIR)/$(FLEXFILE).c -o $(OBJDIR)/$(FLEXFILE).o

$(MODULES):
	$(CC) $(CFLAGS) $(SRCDIR)/$@.cpp -o $(OBJDIR)/$@.o 

$(EXECUTABLE):
	$(CC) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OBJECTS)) $(OBJDIR)/$(FLEXFILE).o -o $(BINDIR)/$@

clean: 
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(EXECUTABLE) 
	

