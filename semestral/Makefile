CXX=g++
CXXFLAGS=-std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb -D __PROGTEST__
CXXFLAGSR=-std=c++17 -Wall -pedantic -Wno-long-long -O0 -ggdb
LDFLAGS=-std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb -D __PROGTEST__
LDFLAGSR=-std=c++17 -Wall -pedantic -Wno-long-long -O0 -ggdb
LDFLAGSRS=-std=c++17 -Wall -pedantic -Wno-long-long -O0 -ggdb -fsanitize=address
PTFLAGS=
PTFLAGSR=-lstdc++fs

all: ancinpet doc

run: 
	./ancinpet

test:
	valgrind --leak-check=full --show-leak-kinds=all ./ancinpet < examples/data1
	./ancinpet < examples/data1

compile: ancinpet

ancinpet: src/main.o src/explorer.o src/command.o src/history.o src/file.o
	$(CXX) $(LDFLAGS) src/main.o src/explorer.o src/command.o src/history.o src/file.o -o ancinpet $(PTFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f ancinpet *.o *~ core src/*.o src/*~ src/core
	rm -r -f doc

doc: docs

docs:
	doxygen Doxyfile

src/command.o: src/command.cpp src/command.hpp src/file.hpp
src/explorer.o: src/explorer.cpp src/explorer.hpp
src/file.o: src/file.cpp src/file.hpp
src/history.o: src/history.cpp src/history.hpp src/command.hpp
src/main.o: src/main.cpp src/main.hpp src/explorer.hpp src/command.hpp src/history.hpp src/file.hpp