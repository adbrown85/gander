prefix     := /usr/local
bindir     := $(prefix)/bin

CXX        := g++
CXXFLAGS   := -g
INCLUDES   := `pkg-config --cflags rapidgl-0`
CXXOPTS    := $(CXXFLAGS) $(INCLUDES)
LDFLAGS    :=
LIBS       := `pkg-config --libs libglfw rapidgl-0`
LDOPTS     := $(LDFLAGS) $(LIBS)
objects    := Picker.o Sphere.o Window.o

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

# Dependencies
