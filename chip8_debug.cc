#include <ncurses.h>
#include <map>
#include <unistd.h>
#include <list>
#include <algorithm>
#include <string.h>
#include <math.h>
#include <iostream>
#include <signal.h>
#include "keyboard.h"

bool teriminated = false;

#include "chip8.h" // Your cpu core implementation
#include "chip8_decode.h" // Your cpu core implementation

chip8 myChip8;

int max_x, max_y;

void  setupGraphics(){
}
void setupInput() {
}
int delay =10000;
void drawGraphics(int mx, int my, unsigned char * array) {
	clear();
	for (int y=0; y <my; y++)  {
		for (int x=0; x <mx; x++)  {
			//attron(COLOR_PAIR(5));
			if (array[x + (y * mx)] )
				printf("%c", 'A');
			else 
				printf(" ");
		}
		printf("\n");
	}
}

void term (int sig) {
   teriminated = true;
}

std::map<std::string, std::function<void()> > commands = 
{
	{"quit", [](){ teriminated = true;}},
	{"n", [](){ myChip8.emulateCycle(); }},
	{"regs", [](){ myChip8.dumpRegs();}}	
};

int main(int argc, char **argv)
{
	const char *rom= "pong.ch8";
	if (argc >=2)
		rom = argv[1];

  signal(SIGTERM, term);
  // Set up render system and register input callbacks
  setupGraphics();
  setupInput();

  // Initialize the Chip8 system and load the game into the memory
  myChip8.initialize();
  myChip8.loadGame(rom);


	ChipInstructionDescription desc;
  // Emulation loop
  while (!myChip8.isDone() && teriminated == false) {
		unsigned short opcode = myChip8.getCurrentOpcode();
		desc.interprete(opcode, stdout);
		printf("\n:");
		fflush(stdout);
		char *command= NULL;
		size_t size=1024;
		size_t rsize =  getline (&command, &size, stdin);
		if (rsize > 0)
			command[rsize-1] = '\0';
		
		if (commands.count(command) > 0 ) {
			commands[command]();
		}
		else 
			printf("unknown command: %s", command);
		free(command);
		
		/*
    // Emulate one cycle
    myChip8.emulateCycle();

    // If the draw flag is set, update the screen
//    sleep(1);
    // Store key press state (Press and Release)
    if (kbhit()) {
        char c = lgetch();
    	myChip8.setKeys(c);
    }
		*/
  }
  return 0;
}
