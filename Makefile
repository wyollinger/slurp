# executables
CXX=g++
FLEX=flex
STRIP=strip
INSTALL=install

# files - there can be no name collisions between MODULES and FLEX_MODULES
MODULES=slurper retriever eventer uri
OBJECTS=$(MODULES:=.o)
FLEX_MODULE=slurpscanner
FLEX_OBJECT=$(FLEX_MODULE:=.o)
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
LDFLAGS=-lpthread 

#rules
all: release

release: CXXFLAGS += -O3 -pipe -m64 
release: $(FLEX_MODULE) $(MODULES) $(EXECUTABLE)
	$(STRIP) $(BINDIR)/$(EXECUTABLE)

debug: CXXFLAGS += -g
debug: $(FLEX_MODULE) $(MODULES) $(EXECUTABLE)

clean: 
	rm -f $(OBJDIR)/*.o $(SRCDIR)/$(FLEX_MODULE).cpp $(BINDIR)/$(EXECUTABLE) 

install:
	install $(BINDIR)/$(EXECUTABLE) $(INSTALLPATH)

$(FLEX_MODULE):
	$(FLEX) -o$(SRCDIR)/$@.cpp $(FLEXDIR)/$@.flex 
	$(CXX) $(CXXFLAGS) -o$(OBJDIR)/$@.o $(SRCDIR)/$@.cpp

$(MODULES):
	$(CXX) $(CXXFLAGS) -o$(OBJDIR)/$@.o $(SRCDIR)/$@.cpp 

$(EXECUTABLE):
	$(CXX) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OBJECTS)) $(addprefix $(OBJDIR)/,$(FLEX_OBJECT)) -o $(BINDIR)/$@

	

