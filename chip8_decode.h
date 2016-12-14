class ChipInstructionDescription {
	public:
		void describe(unsigned short opcode, FILE * fid) {
			switch(opcode & 0xF000) {
				case 0x0000:
					switch(opcode) {
						case 0x00E0: fprintf(fid,"clear screen" );
							break; 
						case 0x00EE: fprintf(fid,"return");
							break; 
						default: 
							fprintf(fid,"unknown opcode");
							
					};
					break;
				case 0x1000: fprintf(fid,"1NNN	Flow	goto NNN;	Jumps to address NNN.");
					return;
				case 0x2000:  fprintf(fid,"2NNN	Flow	*(0xNNN)()	Calls subroutine at NNN.");
					return; // notice return here
				case 0x3000:   fprintf(fid,"3XNN	Cond	if(Vx==NN)	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)");
					break;
				case 0x4000: fprintf(fid,"4XNN	Cond	if(Vx!=NN)	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)");
					break;
				case 0x5000: fprintf(fid,"5XY0	Cond	if(Vx==Vy)	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)");
					break;
				case 0x6000: fprintf(fid,"6XNN	Const	Vx = NN	Sets VX to NN.");
					break; 
				case 0x7000: fprintf(fid,"  7XNN	Const	Vx += NN	Adds NN to VX.");
					break;
				case 0x8000:
					handle8(opcode, fid);
					break;
				case 0x9000: fprintf(fid,"9XY0	Cond	if(Vx!=Vy)	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)");
					break;
				case 0xA000: fprintf(fid," ANNN	MEM	I = NNN	Sets I to the address NNN.");
					break;
				case 0xB000: fprintf(fid," BNNN	Flow	PC=V0+NNN	Jumps to the address NNN plus V0.");
					return;
				case 0xC000: fprintf(fid,"CXNN	Rand	Vx=rand()&NN	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.");
					break;
				case 0xD000:  fprintf(fid,"DXYN	Disp	draw(Vx,Vy,N)	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesnât change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesnât happen");
					break;
				case 0xE000: 
					if ((opcode &0xFF) == 0x9E) {
						fprintf(fid,"EX9E	KeyOp	if(key()==Vx)	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)");
					} else { fprintf(fid," EXA1	KeyOp	if(key()!=Vx)	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)");
					}
					break;
				case 0xF000:
					handleF(opcode, fid);
					break;
				default: 
				  fprintf(fid, "known opcode");
			};
		}
		void handle8(unsigned short opcode, FILE * fid) {
			switch(opcode &0xF) {
				case 0: fprintf(fid,"8XY0	Assign	Vx=Vy	Sets VX to the value of VY.");
					break;
				case 1: fprintf(fid,"8XY1	BitOp	Vx=Vx|Vy	Sets VX to VX or VY. (Bitwise OR operation)");
					break;
				case 2: fprintf(fid,"8XY2	BitOp	Vx=Vx&Vy	Sets VX to VX and VY. (Bitwise AND operation)");
					break;
				case 3: fprintf(fid,"8XY3	BitOp	Vx=Vx^Vy	Sets VX to VX xor VY.");
					break;
				case 4: fprintf(fid,"8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.");
					break;
				case 5: fprintf(fid,"8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.");
					break;
				case 6: fprintf(fid,"8XY6	BitOp	Vx >> 1	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]");
					break;
				case 7: fprintf(fid,"8XY7	Math	Vx=Vy-Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.");
					break;
				case 0xE: fprintf(fid,"8XYE	BitOp	Vx << 1	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]");
					break;
			}
		}
		void handleF(unsigned short opcode, FILE * fid) {
			switch (opcode &0xFF) {
				case 0x07: fprintf(fid," FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer.");
					break;	
				case 0x0A: fprintf(fid," FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)");
					break;
				case 0x15: fprintf(fid," FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX.");
					break;
				case 0x18: fprintf(fid,"FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX.");
					break;
				case 0x1E: fprintf(fid,"FX1E	MEM	I +=Vx	Adds VX to I.[3]");
					break;
				case 0x29: fprintf(fid," FX29	MEM	I=sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.");
					break;
				case 0x33: fprintf(fid,"FX33	BCD	set_BCD(Vx); *(I+0)=BCD(3); *(I+1)=BCD(2); *(I+2)=BCD(1);");
					break;
				case 0x55: fprintf(fid,"FX55	MEM	reg_dump(Vx,&I)	Stores V0 to VX (including VX) in memory starting at address I.[4]");
					break;
				case 0x65: fprintf(fid," FX65	MEM	reg_load(Vx,&I)	Fills V0 to VX (including VX) with values from memory starting at address I.[4]");
					break;
			}
		}
};
