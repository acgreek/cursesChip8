#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <getopt.h>
#include "chip8_decode.h"


int main (int argc, char * argv[]) {
	int option;
	bool doDescription=false;
	bool doHex=false;
	bool doInt=false;
	bool doAsm=false;
	while (-1 != (option = getopt(argc,argv, "adhi"))) {
		switch(option) {
			case 'd':
				doDescription= true;
				break;
			case 'a':
				doAsm=true;
				break;
			case 'h':
				doHex= true;
				break;
			case 'i':
				doInt= true;
				break;
		}
	}
	if (optind == argc) {
		fprintf(stderr, "requires arg\n");
		exit(-1);
	} 
	ChipInstructionDescription desc;
		
	int fid = open(argv[optind], O_RDONLY);
	if (fid <=0)  {
		fprintf(stderr, "failed to open file: %s\n", strerror(errno));
		exit(-1);
	}
  unsigned short opcode;
	while (2 ==read(fid, &opcode, 2) ) {
		opcode = htons(opcode);
		if (doAsm) {
			desc.decode(opcode, stdout);
			printf(";");
		}
		if (doInt)
			desc.interprete(opcode, stdout);
		if (doHex)
			printf("\t%04X ", opcode);
		if (doDescription)
			desc.describe(opcode, stdout);
		printf("\n");
	}
	close(fid);
	exit(0);
}
