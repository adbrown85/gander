prefix     := /usr/local
bindir     := $(prefix)/bin

CXX        := g++
CXXFLAGS   := -g
INCLUDES   := `pkg-config --cflags rapidgl-0`
CXXOPTS    := $(CXXFLAGS) $(INCLUDES)
LDFLAGS    :=
LIBS       := `pkg-config --libs libglfw rapidgl-0`
LDOPTS     := $(LDFLAGS) $(LIBS)
INSTALL    := install
objects    := Picker.o Sphere.o Window.o \
              BlendNode.o BlendNodeUnmarshaller.o \
              BooleanAndNode.o BooleanAndNodeUnmarshaller.o \
              SortNode.o SortNodeUnmarshaller.o

# Program
all: gander
gander: gander.cxx $(objects)
	@$(CXX) $< $(CXXOPTS) $(LDOPTS) -o $@ $(objects)
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
SortNodeUnmarshaller.o: SortNode.h
