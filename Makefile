all: chip8 asm disasm test
CXXFLAGS = -Wall -ggdb3 -std=c++11
LDFLAGS = -lncurses

chip8:chip8.cc chip8.h keyboard.h
	g++ $(CXXFLAGS) chip8.cc -o chip8 $(LDFLAGS)

asm:asm.cc chip8_decode.h
	g++ $(CXXFLAGS) asm.cc -o asm $(LDFLAGS)

disasm:disasm.cc chip8_decode.h
	g++ $(CXXFLAGS) disasm.cc -o disasm $(LDFLAGS)

test:test.cc
	g++ $(CXXFLAGS) test.cc -o test $(LDFLAGS)
