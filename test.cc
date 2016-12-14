#include <ncurses.h>
#include <unistd.h>
#include <list>
#include <algorithm>
#include <string.h>
#include <math.h>
#include <iostream>
#include <signal.h>
#include <assert.h>
#include "keyboard.h"
#include "chip8_decode.h"

bool terminated=false;

#define UNITTEST
#include "chip8.h" // Your cpu core implementation
 
 
int max_x, max_y;
void drawGraphics(int mx, int my, unsigned char * array) { 
}

void term (int sig) {
  terminated= true;
} 

void testAssignLit() {
	chip8 c8;
  c8.initialize();
  c8.LoadInstruction(0, 0x60FF);
  c8.emulateCycle();
	assert(c8.v[0] == 0xFF);
}
void testAddLit() {
	chip8 c8;
  c8.initialize();
  c8.LoadInstruction(0, 0x7005);
  c8.LoadInstruction(1, 0x7005);
  c8.emulateCycle();
  c8.emulateCycle();
	assert(c8.v[0] == 0x0A);
}
void testAddReg5Lit() {
	chip8 c8;
  c8.initialize();
  c8.LoadInstruction(0, 0x7505);
  c8.LoadInstruction(1, 0x7505);
  c8.emulateCycle();
  c8.emulateCycle();
	assert(c8.v[0] == 0x00);
	assert(c8.v[5] == 0x0A);
}

void testShift() {
	chip8 c8;
	ChipInstructionDescription d;
  c8.initialize();
  c8.LoadInstruction(0, ntohs(d.asm_code_conv("SET V0 5")));
  c8.LoadInstruction(1, ntohs(d.asm_code_conv("LSFT V0 V2")));
  c8.emulateCycle();
  c8.emulateCycle();
	assert(c8.v[0xF] == 1);
	assert(c8.v[0x0] == 2);
}

int main(int argc, char **argv) {
	testAssignLit();
	testAddLit();
	testAddReg5Lit();
	testShift();
  // Initialize the Chip8 system and load the game into the memory  
  return 0;
}
