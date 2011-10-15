# executables
CXX=g++
FLEX=flex
STRIP=strip
INSTALL=install

# files - there can be no name collisions between MODULES and FLEX_MODULES
MODULES=slurper retriever scanner eventer uri
OBJECTS=$(MODULES:=.o)
FLEX_MODULES=slurpscanner
FLEX_OBJECTS=$(FLEX_MODULES:=.o)
EXECUTABLE=slurp

#directories
FLEXDIR=flex
SRCDIR=src
OBJDIR=obj
INCDIR=inc
BINDIR=bin
INSTALLPATH=/usr/local/bin

#flags
CXXFLAGS=-c -Wall -lpthread -I$(INCDIR)
FLEXFLAGS=--read --full 
LDFLAGS=-lpthread 

#rules
all: release

release: CXXFLAGS += -O2 -pipe -m64 
release: $(FLEX_MODULES) $(MODULES) $(EXECUTABLE)
	$(STRIP) $(BINDIR)/$(EXECUTABLE)

debug: CXXFLAGS += -g
debug: $(FLEX_MODULES) $(MODULES) $(EXECUTABLE)

clean: 
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(EXECUTABLE) 

install:
	install $(BINDIR)/$(EXECUTABLE) $(INSTALLPATH)

$(FLEX_MODULES):
	$(FLEX) $(FLEXFLAGS) --header-file=$(INCDIR)/$@.h --outfile=$(SRCDIR)/$@.c $(FLEXDIR)/$@.flex 
	$(CXX) $(CXXFLAGS) $(SRCDIR)/$@.c -o $(OBJDIR)/$@.o

$(MODULES):
	$(CXX) $(CXXFLAGS) $(SRCDIR)/$@.cpp -o $(OBJDIR)/$@.o 

$(EXECUTABLE):
	$(CXX) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OBJECTS)) $(addprefix $(OBJDIR)/,$(FLEX_OBJECTS)) -o $(BINDIR)/$@

	

