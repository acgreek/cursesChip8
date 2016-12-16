#include <unordered_map>
#include <map>
#include <sstream>
#include <vector>
#include <functional>
#include <arpa/inet.h>
struct opt {
	unsigned short opcode;
	unsigned short mask;
	const char *asm_cmd;
	unsigned char regs;
	unsigned char lits;
	const char *description;
};



std::map<unsigned short,std::function<void(unsigned short,FILE*)> >    intmap = 
{
{0x00E0, []( unsigned short opcode, FILE * fid) {fprintf(fid, "CLEAR SCREEN");} },
{0x00EE, []( unsigned short opcode, FILE * fid) {fprintf(fid, "RETURN"); } },
{0x1000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "GOTO %d", ((opcode &0xfff)-0x200)/2); } },
{0x2000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "CALL %d", ((opcode &0xfff)-0x200)/2);} },
{0x3000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "if (V%d == %d)", (opcode &0xf00) >> 8, opcode &0xFF);} },
{0x4000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "if (V%d != %d)", (opcode &0xf00) >> 8, opcode &0xFF);} },
{0x5000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "if (V%d == V%d)", (opcode &0xf00) >> 8, (opcode &0xF0)>4 );} },
{0x6000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d = %d", (opcode &0xf00) >> 8, opcode &0xFF);} },
{0x7000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d += %d", (opcode &0xf00) >> 8, opcode &0xFF);} },
{0x8000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d = V%d", (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x8001, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d |= V%d", (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x8002, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d &= V%d", (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x8003, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d =V%d ^ V%d", (opcode &0xf00) >> 8, (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x8004, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d += V%d", (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x8005, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d -= V%d", (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x8006, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d = V%d", (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x8007, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d = V%d", (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x800E, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d = V%d", (opcode &0xf00) >> 8, (opcode &0xF0) >>4);} },
{0x9000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "if (V%d != %d)", (opcode &0xf00) >> 8, opcode &0xFF);} },
{0xA000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "I = %d", (opcode &0xfff));} },
{0xB000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "pc = V0 + %d", (opcode &0xfff));} },
{0xC000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%di = rand() & 0x%X",(opcode &0xf00) >> 8, opcode &0xFF );} },
{0xD000, []( unsigned short opcode, FILE * fid) {fprintf(fid, "DRAW at V%d by V%d hight %d",(opcode &0xf00) >> 8,(opcode &0xf0) >> 4, opcode &0xF );} },
{0xE09E, []( unsigned short opcode, FILE * fid) {fprintf(fid, "if (key() == %d)", (opcode &0xf00) >> 8);} },
{0xE0A1, []( unsigned short opcode, FILE * fid) {fprintf(fid, "if (key() != %d)", (opcode &0xf00) >> 8);} },
{0xF007, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d = delay_timer", (opcode &0xf00) >> 8);} },
{0xF00A, []( unsigned short opcode, FILE * fid) {fprintf(fid, "V%d = getchar()", (opcode &0xf00) >> 8);} },
{0xF015, []( unsigned short opcode, FILE * fid) {fprintf(fid, "delay_timer = V%d", (opcode &0xf00) >> 8);} },
{0xF018, []( unsigned short opcode, FILE * fid) {fprintf(fid, "sound_timer = V%d", (opcode &0xf00) >> 8);} },
{0xF01E, []( unsigned short opcode, FILE * fid) {fprintf(fid, "I += V%d",(opcode &0xf00) >> 8);} },
{0xF029, []( unsigned short opcode, FILE * fid) {fprintf(fid, "I = sprite[V%d]",(opcode &0xf00) >> 8);} },
{0xF033, []( unsigned short opcode, FILE * fid) {fprintf(fid, "SETBCD(V%d)",(opcode &0xf00) >> 8);} },
{0xF055, []( unsigned short opcode, FILE * fid) {fprintf(fid, "reg_dump(V%d,I)", (opcode &0xf00) >> 8);} },
{0xF065, []( unsigned short opcode, FILE * fid) {fprintf(fid, "reg_load(V%d,I)", (opcode &0xf00) >> 8);} },
};

struct opt opts[] = {
{0x00E0, 0xFFFF, "CLRS", 0,0, "clear screen"},
{0x00EE, 0xFFFF,"RETN", 0,0, "return"},
{0x1000, 0xF000,"GOTO", 0,3, "1NNN	Flow	goto NNN;	Jumps to address NNN."},
{0x2000, 0xF000,"CALL", 0,3, "2NNN	Flow	*(0xNNN)()	Calls subroutine at NNN."},
{0x3000, 0xF000,"EQ", 1,2, "3XNN	Cond	if(Vx==NN)	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)"},
{0x4000, 0xF000,"!EQ", 1,2, "4XNN	Cond	if(Vx!=NN)	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)"},
{0x5000, 0xF00F,"EQ", 2, 0, "5XY0	Cond	if(Vx==Vy)	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)"},
{0x6000, 0xF000,"SET", 1, 2, "6XNN	Const	Vx = NN	Sets VX to NN."},
{0x7000, 0xF000,"ADD", 1, 2, "7XNN	Const	Vx += NN	Adds NN to VX."},
{0x8000, 0xF00F,"ASN", 2, 0, "8XY0	Assign	Vx=Vy	Sets VX to the value of VY."},
{0x8001, 0xF00F,"OR", 2, 0, "8XY1	BitOp	Vx=Vx|Vy	Sets VX to VX or VY. (Bitwise OR operation)"},
{0x8002, 0xF00F,"AND", 2, 0, "8XY2	BitOp	Vx=Vx&Vy	Sets VX to VX and VY. (Bitwise AND operation)"},
{0x8003, 0xF00F,"XOR", 2, 0, "8XY3	BitOp	Vx=Vx^Vy	Sets VX to VX xor VY."},
{0x8004, 0xF00F,"ADD", 2, 0, "8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't."},
{0x8005, 0xF00F,"SUB", 2, 0, "8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't."},
{0x8006, 0xF00F,"LSFT", 2, 0, "8XY6	BitOp	Vx >> 1	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]"},
{0x8007, 0xF00F,"SUBFR", 2, 0, "8XY7	Math	Vx=Vy-Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't."},
{0x800E, 0xF00F,"RSFT", 2, 0, "8XYE	BitOp	Vx << 1	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]"},
{0x9000, 0xF00F,"!EQ", 2, 0, "9XY0	Cond	if(Vx!=Vy)	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)"},
{0xA000, 0xF000,"ISET", 0, 3, "ANNN	MEM	I = NNN	Sets I to the address NNN."},
{0xB000, 0xF000,"JMP", 0, 3, "BNNN	Flow	PC=V0+NNN	Jumps to the address NNN plus V0."},
{0xC000, 0xF000,"RAND", 1, 2, "CXNN	Rand	Vx=rand()&NN	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN."},
{0xD000, 0xF000,"DISP", 2, 1, "DXYN	Disp	draw(Vx,Vy,N)	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn't change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn't happen"},
{0xE09E, 0xF0FF,"KEY", 1, 0, "EX9E	KeyOp	if(key()==Vx)	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)"},
{0xE0A1, 0xF0FF,"!KEY", 1, 0, "EXA1	KeyOp	if(key()!=Vx)	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)"},
{0xF007, 0xF0FF,"GETD", 1, 0, "FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer."},
{0xF00A, 0xF0FF,"KEYPRES", 1, 0, "FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)"},
{0xF015, 0xF0FF,"SETD", 1, 0, "FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX."},
{0xF018, 0xF0FF,"SETSND", 1, 0, "FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX."},
{0xF01E, 0xF0FF,"IADD", 1, 0, "FX1E	MEM	I +=Vx	Adds VX to I.[3]"},
{0xF029, 0xF0FF,"ISPR", 1, 0, "FX29	MEM	I=sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font."},
{0xF033, 0xF0FF,"SETBCD", 1, 0, "FX33	BCD	set_BCD(Vx); *(I+0)=BCD(3); *(I+1)=BCD(2); *(I+2)=BCD(1);"},
{0xF055, 0xF0FF,"DUMPREG", 1, 0, "FX55	MEM	reg_dump(Vx,&I)	Stores V0 to VX (including VX) in memory starting at address I.[4]"},
{0xF065, 0xF0FF,"LOADREG", 1, 0, "FX65	MEM	reg_load(Vx,&I)	Fills V0 to VX (including VX) with values from memory starting at address I.[4]"}
};

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

class ChipInstructionDescription {
	private:
		typedef std::unordered_map<unsigned short, struct opt> optMap;
		std::map<unsigned short, optMap> optMaskMap;
		std::map<std::string ,std::map<char, std::map <bool, struct opt> > > asmToOpt;
	public:
 	  ChipInstructionDescription  () {
			for (unsigned i=0; i < sizeof(opts)/sizeof (struct opt); i++) {
				optMaskMap[opts[i].mask] [opts[i].opcode] = opts[i];
				asmToOpt[opts[i].asm_cmd][opts[i].regs][opts[i].lits >0 ? true : false] = opts[i];
			}
		}
		void asm_code(std::string asmcmd, FILE * fid) {
			unsigned short opcode = asm_code_conv(asmcmd);
			fwrite(&opcode, 2,1, fid);
		}
		unsigned short asm_code_conv(std::string asmcmd) {
			std::vector<std::string> lvector; 
			split (asmcmd,  ';',lvector); 
			if (lvector.size () == 0 )
				return 0;
			std::vector<std::string> cvector; 
			split (lvector[0],  ' ',cvector); 
			if (cvector.size () == 0 )
				return 0;
			int regs=0;
			unsigned char reg[3];
			bool hasLit=false;
			unsigned short lit;
			std::vector<std::string>::iterator itr = cvector.begin();
			itr++;
			while (itr != cvector.end()) {
				if (itr->c_str()[0] == 'V') {
					reg[regs] = strtol(itr->c_str()+1, NULL, 10);
					regs++;
				}
				else {
					hasLit=true;
					lit = strtol(itr->c_str(), NULL, 10);
					break;
				}
				itr++;
			}

			struct opt & copt = asmToOpt[cvector[0] ][regs][hasLit];
			unsigned short opcode= copt.opcode & copt.mask;

			if (copt.regs == 1)  {
				opcode |= (reg[0] << 8);
			}
			if (copt.regs == 2)  {
				opcode |= (reg[0] << 8);
				opcode |= (reg[1] << 4);
			}
			if (copt.lits == 3)  {
				opcode |= lit & 0xFFF;
			}
			if (copt.lits == 2)  {
				opcode |= lit & 0x0FF;
			}
			if (copt.lits == 1)  {
				opcode |= lit & 0x0F;
			}
			opcode = htons(opcode);
			return opcode;
			
		}
		void interprete(unsigned short opcode, FILE * fid) {
			struct opt * copt= findOpt(opcode);
			if (copt) {
				intmap[copt->opcode](opcode, fid);
				return;
			}
			fprintf(fid, "%40X", opcode);

		}


		void decode(unsigned short opcode, FILE * fid) {
			struct opt * copt= findOpt(opcode);
			if (copt) {
				fprintf(fid, "%s", copt->asm_cmd);
				if (copt->regs > 0)  {
					fprintf(fid, " V%d", ((opcode& 0xF00) >> 8) & 0xF);
				}
				if (copt->regs > 1)  {
					fprintf(fid, " V%d", ((opcode& 0xF0) >> 4) &0xF);
				}
				if (copt->lits == 3)  {
					fprintf(fid, " %d", (opcode& 0xFFF));
				}
				if (copt->lits == 2)  {
					fprintf(fid, " %d", (opcode& 0xFF));
				}
				if (copt->lits == 1)  {
					fprintf(fid, " %d", (opcode& 0xFF));
				}
				return ;
			}
			fprintf(fid, "%40X", opcode);
			
		}

		void describe(unsigned short opcode, FILE * fid) {
			struct opt * copt= findOpt(opcode);
			if (copt) {
				fprintf(fid, "%s", copt->description);
				return;
			}
			fprintf(fid, "unknown opcode");
		}
	private: 
		struct opt * findOpt(unsigned short opcode) {
			std::map<unsigned short, optMap>::iterator itr = optMaskMap.begin(); 
			while (itr != optMaskMap.end()) {
				if (1 == itr->second.count(itr->first & opcode)) {
					return & optMaskMap[itr->first] [opcode & itr->first];
				}
				itr++;
			}
			return NULL;
		}

};
