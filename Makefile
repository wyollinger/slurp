# executables
CXX=g++
FLEX=flex
STRIP=strip

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
CXXFLAGS=-c -Wall -lpthread -I$(INCDIR)
FLEXFLAGS=--read --full --header-file=$(INCDIR)/$(FLEXFILE).h --outfile=$(SRCDIR)/$(FLEXFILE).c
LDFLAGS=-lpthread 

#rules
all: release

release: CXXFLAGS += -O3 -pipe 
release: $(FLEXFILE) $(MODULES) $(EXECUTABLE)
	$(STRIP) $(BINDIR)/$(EXECUTABLE)

debug: CXXFLAGS += -g
debug: $(FLEXFILE) $(MODULES) $(EXECUTABLE)

$(FLEXFILE):
	$(FLEX) $(FLEXFLAGS) $(FLEXDIR)/$(FLEXFILE).flex 
	$(CXX) $(CXXFLAGS) $(SRCDIR)/$(FLEXFILE).c -o $(OBJDIR)/$(FLEXFILE).o

$(MODULES):
	$(CXX) $(CXXFLAGS) $(SRCDIR)/$@.cpp -o $(OBJDIR)/$@.o 

$(EXECUTABLE):
	$(CXX) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OBJECTS)) $(OBJDIR)/$(FLEXFILE).o -o $(BINDIR)/$@

clean: 
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(EXECUTABLE) 
	

