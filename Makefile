# Directories
prefix     := /usr/local
bindir     := $(prefix)/bin

# Tools
CXX        := g++
INSTALL    := install

# Compiler options
CXXFLAGS   := -g
INCLUDES   := `pkg-config --cflags rapidgl-0`
CXXOPTS    := $(CXXFLAGS) $(INCLUDES)

# Linker options
LDFLAGS    :=
LIBS       := `pkg-config --libs libglfw rapidgl-0` -lPocoFoundation
LDOPTS     := $(LDFLAGS) $(LIBS)

# Files
objects    := Picker.o Sphere.o WindowAdapter.o \
              BlendNode.o BlendNodeUnmarshaller.o \
              BooleanAndNode.o BooleanAndNodeUnmarshaller.o \
              BooleanXorNode.o BooleanXorNodeUnmarshaller.o \
              SortNode.o SortNodeUnmarshaller.o

# Program
all: gander
gander: gander.cxx $(objects)
	@$(CXX) $< $(CXXOPTS) -o $@ $(objects) $(LDOPTS)
clean:
	@$(RM) gander
	@$(RM) *.o
	@$(RM) -r *.dSYM

# Objects
$(objects):
%.o: %.cxx %.h
	@$(CXX) $< $(CXXOPTS) -c -o $@

# Installation
install:
	$(INSTALL) gander $(bindir)
uninstall:
	$(RM) $(bindir)/gander

# Dependencies
BlendNodeUnmarshaller.o: BlendNode.h
BooleanAndNodeUnmarshaller.o: BooleanAndNode.h
BooleanXorNodeUnmarshaller.o: BooleanXorNode.h
SortNodeUnmarshaller.o: SortNode.h
