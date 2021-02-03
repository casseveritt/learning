
WARNFLAGS := -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wshadow

CFLAGS_NOWARN := -g -std=c++11
CFLAGS := $(CFLAGS_NOWARN) $(WARNFLAGS)

all: trig

H := $(wildcard *.h)

SRC := trig.cpp

OBJ := $(patsubst %.cpp, %.o, $(SRC))

.cpp.o:
	g++ $(CFLAGS) $< -c

trig: $(OBJ) $(H)
	g++ $(CFLAGS) $(OBJ) -o trig -lpthread

format: format_h format_cpp

format_h: $(H)
	for h in $^; do clang-format -style=file -i $$h; done

format_cpp: $(SRC)
	for cpp in $^; do clang-format -style=file -i $$cpp; done

clean:
	rm -f *.o
	rm -f trig
	rm -f *~
	rm -f .*~


