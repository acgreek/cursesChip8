#include <ncurses.h>
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

chip8 myChip8;

int max_x, max_y;

void  setupGraphics(){
	initscr();
	if(has_colors() == FALSE)
	{
		endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
	start_color();
	init_pair(1, COLOR_RED, COLOR_RED);
	init_pair(2, COLOR_BLUE, COLOR_BLUE);
	init_pair(3, COLOR_GREEN, COLOR_GREEN);
	init_pair(4, COLOR_WHITE, COLOR_WHITE);
	init_pair(5, COLOR_WHITE, COLOR_BLACK);
	wbkgd(stdscr, COLOR_PAIR(2));
	noecho();
	curs_set(FALSE);
	srand (time(NULL));
	getmaxyx(stdscr, max_y, max_x);
}
void setupInput() {
}
int delay =10000;
void drawGraphics(int mx, int my, unsigned char * array) {
    clear();
   	for (int y=0; y <my; y++)  {
   	 for (int x=0; x <mx; x++)  {
		attron(COLOR_PAIR(3));
		if (array[x + (y * mx)] )
	   		mvprintw(y, x, "%c", 'A');
//		 	printf("%c",array[x + (y * mx)] ? 'X': ' ');
     }
//		 printf("\n");
    }
	attron(COLOR_PAIR(5));
	wnoutrefresh(stdscr);
	doupdate();
	usleep(delay);
}

void term (int sig) {
   teriminated = true;
}

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

  // Emulation loop
  while (!myChip8.isDone() && teriminated == false) {

    // Emulate one cycle
    myChip8.emulateCycle();

    // If the draw flag is set, update the screen
    if(myChip8.drawFlag) {
      drawGraphics(myChip8.getMaxX(),myChip8.getMaxY(), myChip8.getGfx() );
    }

//    sleep(1);
    // Store key press state (Press and Release)
    if (kbhit()) {
        char c = lgetch();
    	myChip8.setKeys(c);
    }
  }

  endwin();
  return 0;
}
