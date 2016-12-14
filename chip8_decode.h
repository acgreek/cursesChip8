#include <unordered_map>
#include <map>
struct opt {
	unsigned short opcode;
	unsigned short mask;
	unsigned char regs;
	unsigned char lits;
	const char *description;
};

struct opt opts[] = {
{0x00E0, 0xFFFF, 0,0, "clear screen"},
{0x00EE, 0xFFFF, 0,0, "return"},
{0x1000, 0xF000, 0,3, "1NNN	Flow	goto NNN;	Jumps to address NNN."},
{0x2000, 0xF000, 0,3, "2NNN	Flow	*(0xNNN)()	Calls subroutine at NNN."},
{0x3000, 0xF000, 1,2, "3XNN	Cond	if(Vx==NN)	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)"},
{0x4000, 0xF000, 1,2, "4XNN	Cond	if(Vx!=NN)	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)"},
{0x5000, 0xF00F, 2, 0, "5XY0	Cond	if(Vx==Vy)	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)"},
{0x6000, 0xF000, 1, 2, "6XNN	Const	Vx = NN	Sets VX to NN."},
{0x7000, 0xF000, 1, 2, "7XNN	Const	Vx += NN	Adds NN to VX."},
{0x8000, 0xF00F, 2, 0, "8XY0	Assign	Vx=Vy	Sets VX to the value of VY."},
{0x8001, 0xF00F, 2, 0, "8XY1	BitOp	Vx=Vx|Vy	Sets VX to VX or VY. (Bitwise OR operation)"},
{0x8002, 0xF00F, 2, 0, "8XY2	BitOp	Vx=Vx&Vy	Sets VX to VX and VY. (Bitwise AND operation)"},
{0x8003, 0xF00F, 2, 0, "8XY3	BitOp	Vx=Vx^Vy	Sets VX to VX xor VY."},
{0x8004, 0xF00F, 2, 0, "8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't."},
{0x8005, 0xF00F, 2, 0, "8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't."},
{0x8006, 0xF00F, 2, 0, "8XY6	BitOp	Vx >> 1	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]"},
{0x8007, 0xF00F, 2, 0, "8XY7	Math	Vx=Vy-Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't."},
{0x8008, 0xF00F, 2, 0, "8XYE	BitOp	Vx << 1	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]"},
{0x9000, 0xF00F, 2, 0, "9XY0	Cond	if(Vx!=Vy)	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)"},
{0xA000, 0xF000, 0, 3, "ANNN	MEM	I = NNN	Sets I to the address NNN."},
{0xB000, 0xF000, 0, 3, "BNNN	Flow	PC=V0+NNN	Jumps to the address NNN plus V0."},
{0xC000, 0xF000, 1, 2, "CXNN	Rand	Vx=rand()&NN	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN."},
{0xD000, 0xF000, 2, 1, "DXYN	Disp	draw(Vx,Vy,N)	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn't change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn't happen"},
{0xE09E, 0xF0FF, 1, 0, "EX9E	KeyOp	if(key()==Vx)	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)"},
{0xE0A1, 0xF0FF, 1, 0, "EXA1	KeyOp	if(key()!=Vx)	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)"},
{0xF007, 0xF0FF, 1, 0, "FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer."},
{0xF00A, 0xF0FF, 1, 0, "FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)"},
{0xF015, 0xF0FF, 1, 0, "FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX."},
{0xF018, 0xF0FF, 1, 0, "FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX."},
{0xF01E, 0xF0FF, 1, 0, "FX1E	MEM	I +=Vx	Adds VX to I.[3]"},
{0xF029, 0xF0FF, 1, 0, "FX29	MEM	I=sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font."},
{0xF033, 0xF0FF, 1, 0, "FX33	BCD	set_BCD(Vx); *(I+0)=BCD(3); *(I+1)=BCD(2); *(I+2)=BCD(1);"},
{0xF055, 0xF0FF, 1, 0, "FX55	MEM	reg_dump(Vx,&I)	Stores V0 to VX (including VX) in memory starting at address I.[4]"},
{0xF065, 0xF0FF, 1, 0, "FX65	MEM	reg_load(Vx,&I)	Fills V0 to VX (including VX) with values from memory starting at address I.[4]"}
};


class ChipInstructionDescription {
  private:
		typedef std::unordered_map<unsigned short, struct opt> optMap;
		std::map<unsigned short, optMap> optMaskMap;
	public:
 	  ChipInstructionDescription  () {
			for (unsigned i=0; i < sizeof(opts)/sizeof (struct opt); i++) {
				optMaskMap[opts[i].mask] [opts[i].opcode] = opts[i];
			}
		}
		void describe(unsigned short opcode, FILE * fid) {
			std::map<unsigned short, optMap>::iterator itr = optMaskMap.begin(); 
			while (itr != optMaskMap.end()) {
				if (1 == itr->second.count(itr->first & opcode)) {
					//fprintf(fid, "%s", itr->second[itr->first & opcode].description);
					auto foo = optMaskMap[itr->first] [opcode & itr->first];
					fprintf(fid, "%s", foo.description);
					return;
				}
				itr++;
			}
			fprintf(fid, "unknown opcode");
		}
};
