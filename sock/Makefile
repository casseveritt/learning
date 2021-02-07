
WARNFLAGS := -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wshadow

CFLAGS_NOWARN := -g -std=c++11
CFLAGS := $(CFLAGS_NOWARN) $(WARNFLAGS)

all: server client

H := $(wildcard *.h)

SRC := sock.cpp socket.cpp

OBJ := $(patsubst %.cpp, %.o, $(SRC))

.cpp.o:
	g++ $(CFLAGS) $< -c

server: $(OBJ) $(H)
	g++ $(CFLAGS) $(OBJ) -o server -lpthread

client: $(OBJ) $(H)
	g++ $(CFLAGS) $(OBJ) -o client -lpthread

format: format_h format_cpp

format_h: $(H)
	for h in $^; do clang-format -style=file -i $$h; done

format_cpp: $(SRC)
	for cpp in $^; do clang-format -style=file -i $$cpp; done

clean:
	rm -f *.o
	rm -f server
	rm -f client
	rm -f *~
	rm -f .*~


