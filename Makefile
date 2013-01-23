CXX        := g++
CXXFLAGS   := -g
INCLUDES   := `pkg-config --cflags rapidgl-0`
CXXOPTS    := $(CXXFLAGS) $(INCLUDES)
LDFLAGS    :=
LIBS       := -lglfw -framework OpenGL -framework Cocoa `pkg-config --libs rapidgl-0`
LDOPTS     := $(LDFLAGS) $(LIBS)

# Program
all: gander
gander: gander.cxx
	@$(CXX) $< $(CXXOPTS) $(LDOPTS) -o $@
clean:
	@$(RM) gander
