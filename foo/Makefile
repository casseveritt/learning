
foo: foo.o
	g++ -o foo foo.o -lfmt

foo.o: foo.cpp
	g++ --std=c++17 -I ../include -c foo.cpp

clean:
	rm -f foo.o
	rm -f foo
