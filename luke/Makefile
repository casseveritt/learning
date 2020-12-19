
UNAME := $(shell uname)
$(info UNAME is $(UNAME))

WARNFLAGS := -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow  -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef

CFLAGS_NOWARN := -g -isystem stb -std=c++11
CFLAGS := $(CFLAGS_NOWARN) $(WARNFLAGS)

ifeq ($(UNAME),Darwin)
	GLFLAGS := -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon
else
	GLFLAGS := -lGLESv2
endif

all: gl

H := $(wildcard *.h)

SRC := glfw.cpp gl.cpp geom.cpp sphere.cpp cube.cpp torus.cpp prog.cpp square.cpp tetra.cpp

OBJ := $(patsubst %.cpp, %.o, $(SRC))

.cpp.o:
	g++ $(CFLAGS) $< -c

gl: $(OBJ) $(H) stb.o
	g++ $(CFLAGS) $(OBJ) stb.o -o gl $(GLFLAGS) -lglfw

stb.o: stb.cpp
	g++ $(CFLAGS_NOWARN) stb.cpp -c

format: format_h format_cpp

format_h: $(H)
	for h in $^; do clang-format -style=file -i $$h; done

format_cpp: $(SRC)
	for cpp in $^; do clang-format -style=file -i $$cpp; done

clean:
	rm -f *.o
	rm -f gl
	rm -f *~
	rm -f .*~


