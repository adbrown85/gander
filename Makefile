CXX        := g++
CXXFLAGS   := -g
INCLUDES   := `pkg-config --cflags rapidgl-0`
CXXOPTS    := $(CXXFLAGS) $(INCLUDES)
LDFLAGS    :=
LIBS       := `pkg-config --libs libglfw rapidgl-0`
LDOPTS     := $(LDFLAGS) $(LIBS)

# Program
all: gander
gander: gander.cxx
	@$(CXX) $< $(CXXOPTS) $(LDOPTS) -o $@
clean:
	@$(RM) gander
