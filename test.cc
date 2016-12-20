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

void doTwo(chip8 &c8, const char *one, const char *two ) {
	ChipInstructionDescription d;
	c8.initialize();
	c8.LoadInstruction(0, ntohs(d.asm_code_conv(one)));
	c8.LoadInstruction(1, ntohs(d.asm_code_conv(two)));
	c8.emulateCycle();
	c8.emulateCycle();

}
void doThree(chip8 &c8, const char *one, const char *two,const char * three ) {
	ChipInstructionDescription d;
	c8.initialize();
	c8.LoadInstruction(0, ntohs(d.asm_code_conv(one)));
	c8.LoadInstruction(1, ntohs(d.asm_code_conv(two)));
	c8.LoadInstruction(2, ntohs(d.asm_code_conv(three)));
	c8.emulateCycle();
	c8.emulateCycle();
	c8.emulateCycle();

}


void testRightShift() {
	chip8 c8;
	doTwo(c8, "SET V0 5", "RSFT V0 V2");
	assert(c8.v[0xF] == 1);
	assert(c8.v[0x0] == 2);
}
void testRightShiftNoOverFlow() {
	chip8 c8;
	doTwo(c8, "SET V0 4", "RSFT V0 V2");
	assert(c8.v[0xF] == 0);
	assert(c8.v[0x0] == 2);
}
void testLeftShift() {
	chip8 c8;
	doTwo(c8, "SET V0 255", "LSFT V0 V2");
	assert(c8.v[0xF] == 1);
	assert(c8.v[0x0] == 254);
}
void testLeftShiftNoOverFlow() {
	chip8 c8;
	doTwo(c8, "SET V0 127", "LSFT V0 V2");
	assert(c8.v[0xF] == 0);
	assert(c8.v[0x0] == 254);
}
void testEqFalse() {
	chip8 c8;
	doTwo(c8, "SET V0 127", "EQ V0 255");
	assert(c8.pc  == 516);
}
void testEqTrue() {
	chip8 c8;
	doTwo(c8, "SET V0 127", "EQ V0 127");
	assert(c8.pc  == 518);
}
void testNEqFalse() {
	chip8 c8;
	doTwo(c8, "SET V0 127", "!EQ V0 127");
	assert(c8.pc  == 516);
}
void testNEqTrue() {
	chip8 c8;
	doTwo(c8, "SET V0 127", "!EQ V0 3");
	assert(c8.pc  == 518);
}
void testEqV2False() {
	chip8 c8;
	doThree(c8, "SET V0 127","SET V1 255", "EQ V0 V1");
	assert(c8.pc  == 518);
}
void testEqV2True() {
	chip8 c8;
	doThree(c8, "SET V0 127","SET V1 127", "EQ V0 V1");
	assert(c8.pc  == 520);
}



int main(int argc, char **argv) {
	testAssignLit();
	testAddLit();
	testAddReg5Lit();
	testRightShiftNoOverFlow();
	testRightShift();
	testLeftShiftNoOverFlow();
	testLeftShift();
	//conditionals
	testEqTrue();
	testEqFalse();
	testEqV2True();
	testEqV2False();
	testNEqTrue();
	testNEqFalse();
	// Initialize the Chip8 system and load the game into the memory
	return 0;
}
