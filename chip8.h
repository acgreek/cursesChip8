// vim: set ts=2; smartindent; tabstop=2; shiftwidth=2

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
/*
	 0NNN	Call		Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
	 00E0	Display	disp_clear()	Clears the screen.
	 00EE	Flow	return;	Returns from a subroutine.
	 1NNN	Flow	goto NNN;	Jumps to address NNN.
	 2NNN	Flow	*(0xNNN)()	Calls subroutine at NNN.
	 3XNN	Cond	if(Vx==NN)	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
	 4XNN	Cond	if(Vx!=NN)	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
	 5XY0	Cond	if(Vx==Vy)	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
	 6XNN	Const	Vx = NN	Sets VX to NN.
	 7XNN	Const	Vx += NN	Adds NN to VX.
	 8XY0	Assign	Vx=Vy	Sets VX to the value of VY.
	 8XY1	BitOp	Vx=Vx|Vy	Sets VX to VX or VY. (Bitwise OR operation)
	 8XY2	BitOp	Vx=Vx&Vy	Sets VX to VX and VY. (Bitwise AND operation)
	 8XY3	BitOp	Vx=Vx^Vy	Sets VX to VX xor VY.
	 8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
	 8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
	 8XY6	BitOp	Vx >> 1	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]
	 8XY7	Math	Vx=Vy-Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
	 8XYE	BitOp	Vx << 1	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
	 9XY0	Cond	if(Vx!=Vy)	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
	 ANNN	MEM	I = NNN	Sets I to the address NNN.
	 BNNN	Flow	PC=V0+NNN	Jumps to the address NNN plus V0.
	 CXNN	Rand	Vx=rand()&NN	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
	 DXYN	Disp	draw(Vx,Vy,N)	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesnât change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesnât happen
	 EX9E	KeyOp	if(key()==Vx)	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
	 EXA1	KeyOp	if(key()!=Vx)	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
	 FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer.
	 FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
	 FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX.
	 FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX.
	 FX1E	MEM	I +=Vx	Adds VX to I.[3]
	 FX29	MEM	I=sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
	 FX33	BCD	set_BCD(Vx);
 *(I+0)=BCD(3);

 *(I+1)=BCD(2);

 *(I+2)=BCD(1);

 Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
 FX55	MEM	reg_dump(Vx,&I)	Stores V0 to VX (including VX) in memory starting at address I.[4]
 FX65	MEM	reg_load(Vx,&I)	Fills V0 to VX (including VX) with values from memory starting at address I.[4]
 */
static unsigned char chip8_fontset[80] =
{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class chip8 {
	public: 
		chip8() : drawFlag(false) {};
		void initialize() {
			pc     = 0x200;  // Program counter starts at 0x200
			//opcode = 0;      // Reset current opcode	
			I      = 0;      // Reset index register
			sp     = 0;      // Reset stack pointer

			// Clear display	
			// Clear stack
			// Clear registers V0-VF
			// Clear memory

			// Load fontset
			for(int i = 0; i < 80; ++i)
				memory[i] = chip8_fontset[i];		

			// Reset timers
		}
		int getMaxX()const  {
			return 64;
		}
		int getMaxY()const  {
			return 32;
		}
		unsigned char *getGfx() {
			return gfx;
		}

		void loadGame(std::string filename) {
			FILE *fd = fopen(filename.c_str(), "r");
			fread(memory + pc, 1,sizeof(memory) -pc , fd);
			fclose(fd);
		} 

		void emulateCycle() {
			emulateCycle_();
			if (delay_timer) 
				delay_timer--;
			if (sound_timer)
				sound_timer--;

		}
		// Emulate one cycle
		void emulateCycle_() {

			drawFlag = false;
			unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
			short t1,t2;
			int i,j;
			switch(opcode & 0xF000) {
				case 0x0000:
					switch(opcode) {
						case 0x00E0: //clear screen 
//							printf("clear screen\n");
					    drawFlag = true;
							memset(gfx, 0, sizeof(gfx));
							break; 
						case 0x00EE: // return 
							if (sp == 0) {
								printf("sp == 0 at ps %d\n", pc);
								done =true;
							} 
							pc = stack[sp -1] ;
							sp--;
							break; //we want it to move to next instruction 
						default: 
						  if (opcode == 0) {
								done =true;
						  	printf("known opcode: %x \n", opcode);
							} 
					};
					break;
				case 0x1000: //1NNN	Flow	goto NNN;	Jumps to address NNN.
					pc = opcode & 0xFFF;
					return;
				case 0x2000:  //2NNN	Flow	*(0xNNN)()	Calls subroutine at NNN.
					if (sp == sizeof(stack)/sizeof(short))  {
						printf("stack overflow at ps %d\n", pc);
						done =true;
					}
					stack[sp] = pc;
					sp++;
					pc = opcode &0xFFF;
					return; // notice return here
				case 0x3000:   //3XNN	Cond	if(Vx==NN)	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
					t1=(opcode & 0xF00) >>8;
					t2=(opcode & 0xFF);
					if (v[t1] == t2) 
						pc+=2;
					break;
				case 0x4000: //4XNN	Cond	if(Vx!=NN)	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
					t1=(opcode & 0xF00) >>8;
					t2=(opcode & 0xFF);
					if (v[t1] != t2) 
						pc+=2;
					break;
				case 0x5000: //5XY0	Cond	if(Vx==Vy)	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
					t1=(opcode & 0xF00) >>8;
					t2=(opcode & 0xF0 >> 4);
					if (v[t1] == v[t2]) 
						pc+=2;
					break;
				case 0x6000: //6XNN	Const	Vx = NN	Sets VX to NN.
					t1=(opcode & 0xF00) >>8;
					t2=(opcode & 0xFF);
					v[t1]  = t2;
					break; 
				case 0x7000: //  7XNN	Const	Vx += NN	Adds NN to VX.
					t1=(opcode & 0xF00) >>8;
					t2=(opcode & 0xFF);
					v[t1] += t2;
					break;
				case 0x8000:
					handle8(opcode);
					break;
				case 0x9000: //9XY0	Cond	if(Vx!=Vy)	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
					t1 = v[(opcode & 0xF00) >> 8];
					t2 = v[(opcode & 0xF0) >> 4];
					if (t1 != t2) 
						pc+=2;
					break;
				case 0xA000: // ANNN	MEM	I = NNN	Sets I to the address NNN.
					t1=(opcode & 0xFFF);
					I = t1;
					break;
				case 0xB000: // BNNN	Flow	PC=V0+NNN	Jumps to the address NNN plus V0.
					t1=(opcode & 0xFFF) ;
					pc = v[0] +t1;
					return;
				case 0xC000: //CXNN	Rand	Vx=rand()&NN	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
					t1= rand() & 0xFF;
					t2=(opcode & 0xF00) >> 8 ;
					v[t2] = t1;
					break;
				case 0xD000:  //DXYN	Disp	draw(Vx,Vy,N)	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesnât change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesnât happen
					drawFlag = true;

					v[0xF] = 0;
					t1=((opcode & 0xF00) >>8) &0xf;
					t2=((opcode & 0xF0) >> 4) & 0xf;
					for (j=0; j < (opcode &0xF); j++) {
						int pixel = memory[I+ j];
						for (i=0; i< 8; i++) {
							if((pixel & (0x80 >> i)) != 0)
							{
								if(gfx[(v[t1] + i + ((v[t2] + j) * 64))] == 1)
									v[0xF] = 1;                                 
								gfx[(v[t1] + i + ((v[t2] + j) * 64))] ^= 1;
							}
						} 
					}
					break;
				case 0xE000: 
					t1= (opcode & 0xF00) >> 8;
					if ((opcode &0xFF) == 0x9E) {
						if (keypress(v[t1])) //EX9E	KeyOp	if(key()==Vx)	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
							pc+=2;
					} else { // EXA1	KeyOp	if(key()!=Vx)	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
						if (!keypress(v[t1])) 
							pc+=2;
					}
					break;
				case 0xF000:
					handleF(opcode);
					break;
				default: 
				  printf("known opcode: %x \n", opcode);
					done =true;
			};
			pc+=2;
		}
		char keyConvert(char k) {
			char ckey=-1;
			if (k>= '0' && k <= '9')
				ckey = k - '0';
			if (k>= 'a' && k <= 'f')
				ckey = 10 + (k - 'a');
			return ckey;
		}

		void setKeys(char k) {
		  ckey = keyConvert(k);
			if (ckey != -1)
				key[ckey] = 1;
		}
		bool keypress(unsigned short k) {
			if (k >0xF)
				return 0;
			int pressed = key[k];
		  key[k] = 0;
			return pressed;
		}

		// If the draw flag is set, update the screen
		bool drawFlag;
	private:
		short ckey;
		void handle8(unsigned short opcode) {
			unsigned char & Vx = v[(opcode & 0xF00) >> 8];
			unsigned char & Vy = v[(opcode & 0xF0) >> 4];
			switch(opcode &0xF) {
				case 0: //8XY0	Assign	Vx=Vy	Sets VX to the value of VY.
					Vx=Vy;
					break;
				case 1: //8XY1	BitOp	Vx=Vx|Vy	Sets VX to VX or VY. (Bitwise OR operation)
					Vx= Vx| Vy;
					break;
				case 2: //8XY2	BitOp	Vx=Vx&Vy	Sets VX to VX and VY. (Bitwise AND operation)
					Vx= Vx& Vy;
					break;
				case 3: //8XY3	BitOp	Vx=Vx^Vy	Sets VX to VX xor VY.
					Vx= Vx^ Vy;
					break;
				case 4: //8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
					v[0xF] = 0;
					if (((int)Vx +(int) Vy) > USHRT_MAX)
						v[0xF] = 1;
					Vx+= Vy;
					break;
				case 5: //8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
					v[0xF] = 1;
					if (Vx > Vy) 
						v[0xF] = 0;
					Vx-= Vy;
					break;
				case 6: //8XY6	BitOp	Vx >> 1	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]
					v[0xF] = Vx&1;
					Vx= Vx>> 1;
					break;
				case 7: //8XY7	Math	Vx=Vy-Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
					v[0xF] = 1;
					if (Vy < Vx) 
						v[0xF] = 0;
					Vx= Vy - Vx;
					break;
				case 0xE: //8XYE	BitOp	Vx << 1	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
					v[0xF] = (Vx & 0x80) >0;
					Vx= Vx<<1;
					break;
			}
		}
		void handleF(unsigned short opcode) {
			unsigned char & Vx = v[(opcode & 0xF00) >> 8];
			switch (opcode &0xFF) {
				case 0x07: // FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer.
					Vx = delay_timer;
					break;	
				case 0x0A: // FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
					Vx = keyConvert(getchar());
					break;
				case 0x15: // FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX.
					delay_timer =Vx;
					break;
				case 0x18: //FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX.
					sound_timer = Vx;
					break;
				case 0x1E: //FX1E	MEM	I +=Vx	Adds VX to I.[3]
					I +=Vx;
					break;
				case 0x29: // FX29	MEM	I=sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
					I = (5 * Vx);
					break;
				case 0x33:
					/*
						 FX33	BCD	set_BCD(Vx);
					 *(I+0)=BCD(3);
					 *(I+1)=BCD(2);
					 *(I+2)=BCD(1);
					 Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
					 */
					memory[I]     = Vx / 100;
					memory[I + 1] = (Vx / 10) % 10;
					memory[I + 2] = (Vx % 100) % 10;
					break;
				case 0x55: //FX55	MEM	reg_dump(Vx,&I)	Stores V0 to VX (including VX) in memory starting at address I.[4]
					for (int i=0; i<32; i+=2) {
						memory[I+i] = (v[i] >> 8 )& 0xFF;
						memory[I+i+1] = (v[i] & 0xFF);

					}
					break;
				case 0x65: // FX65	MEM	reg_load(Vx,&I)	Fills V0 to VX (including VX) with values from memory starting at address I.[4]
					for (int i=0; i< 32; i+=2) {
						v[i] = (memory[I+i] << 8) ;
						v[i] += memory[I+i+1];
					}
					break;
			}
		}
		unsigned char memory[4096];
		unsigned char v[16];
		unsigned short I=0;
		unsigned short pc=0;
		unsigned char gfx[32*64]; //x -y 
		unsigned char delay_timer;
		unsigned char sound_timer;
		unsigned short stack[16];
		unsigned short sp;
		unsigned char key[16];
};
