all: chip8 chip8_debug asm disasm test
CXXFLAGS = -Wall -ggdb3 -std=c++11
LDFLAGS = -lncurses

chip8:chip8.cc chip8.h keyboard.h
	g++ $(CXXFLAGS) chip8.cc -o chip8 $(LDFLAGS)

chip8_debug:chip8_debug.cc chip8.h keyboard.h
	g++ $(CXXFLAGS) chip8_debug.cc -o chip8_debug $(LDFLAGS)

asm:asm.cc chip8_decode.h
	g++ $(CXXFLAGS) asm.cc -o asm $(LDFLAGS)

disasm:disasm.cc chip8_decode.h
	g++ $(CXXFLAGS) disasm.cc -o disasm $(LDFLAGS)

test:test.cc chip8.h
	g++ $(CXXFLAGS) test.cc -o test $(LDFLAGS)
