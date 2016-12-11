CXXFLAGS = -Wall -ggdb3 -std=c++11
LDFLAGS = -lncurses

chip8:chip8.cc chip8.h keyboard.h
	g++ $(CXXFLAGS)   chip8.cc   -o chip8 $(LDFLAGS)
