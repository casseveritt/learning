
WARNFLAGS := -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused

CFLAGS_NOWARN := -O2 -isystem stb -std=c++11
CFLAGS := $(CFLAGS_NOWARN) $(WARNFLAGS)

all: gl

H := $(wildcard *.h)

SRC := gl.cpp glprog.cpp

OBJ := $(patsubst %.cpp, %.o, $(SRC))

.cpp.o:
	g++ $(CFLAGS) $< -c

gl: $(OBJ) $(H) stb.o
	g++ $(CFLAGS) $(OBJ) stb.o -o gl -lGLESv2 -lglfw

stb.o: stb.cpp
	g++ $(CFLAGS_NOWARN) stb.cpp -c

format: format_h format_cpp

format_h: $(H)
	for h in $^; do clang-format -style=file $$h > tmp.h; mv tmp.h $$h; done

format_cpp: $(SRC)
	for cpp in $^; do clang-format -style=file $$cpp > tmp.cpp; mv tmp.cpp $$cpp; done



clean:
	rm -f *.o
	rm -f gl
	rm -f *~
	rm -f .*~


