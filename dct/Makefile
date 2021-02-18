
WARNFLAGS := -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow  -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef

CFLAGS_NOWARN := -g -isystem stb -std=c++11
CFLAGS := $(CFLAGS_NOWARN) $(WARNFLAGS)

all: dct

H := $(wildcard *.h)

SRC := dct.cpp

OBJ := $(patsubst %.cpp, %.o, $(SRC))

.cpp.o:
	g++ $(CFLAGS) $< -c

dct: $(OBJ) $(H) stb.o
	g++ $(CFLAGS) $(OBJ) stb.o -o dct

stb.o: stb.cpp
	g++ $(CFLAGS_NOWARN) stb.cpp -c

format: format_h format_cpp

format_h: $(H)
	for h in $^; do clang-format -style=file -i $$h; done

format_cpp: $(SRC)
	for cpp in $^; do clang-format -style=file -i $$cpp; done

clean:
	rm -f *.o
	rm -f dct
	rm -f *~
	rm -f .*~


