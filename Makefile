CXX        := g++
CXXFLAGS   := -g
INCLUDES   := `pkg-config --cflags rapidgl-0`
CXXOPTS    := $(CXXFLAGS) $(INCLUDES)
LDFLAGS    :=
LIBS       := `pkg-config --libs libglfw rapidgl-0`
LDOPTS     := $(LDFLAGS) $(LIBS)

# Program
all: gander
gander: gander.cxx Window.o
	@$(CXX) $< $(CXXOPTS) $(LDOPTS) -o $@ Window.o
clean:
	@$(RM) gander
Window.o: Window.cxx Window.h
	@$(CXX) $< $(CXXOPTS) -c -o $@
