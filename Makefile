CXX        := g++
CXXFLAGS   := -g
INCLUDES   := `pkg-config --cflags rapidgl-0`
CXXOPTS    := $(CXXFLAGS) $(INCLUDES)
LDFLAGS    :=
LIBS       := `pkg-config --libs libglfw rapidgl-0`
LDOPTS     := $(LDFLAGS) $(LIBS)
objects    := Sphere.o Window.o

# Program
all: gander
gander: gander.cxx $(objects)
	@$(CXX) $< $(CXXOPTS) $(LDOPTS) -o $@ $(objects)
clean:
	@$(RM) gander

# Objects
$(objects):
%.o: %.cxx %.h
	@$(CXX) $< $(CXXOPTS) -c -o $@

# Dependencies
Sphere.o: Ray.h
