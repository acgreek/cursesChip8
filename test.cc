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


int main(int argc, char **argv) {
	testAssignLit();
	testAddLit();
	testAddReg5Lit();
  // Initialize the Chip8 system and load the game into the memory  
  return 0;
}
