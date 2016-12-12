#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "chip8_decode.h"


int main (int argc, char * argv[]) {
	if (argc < 2) {
		fprintf(stderr, "requires arg\n");
		exit(-1);
	} 
	ChipInstructionDescription desc;
		
	int fid = open(argv[1], O_RDONLY);
	if (fid <=0)  {
		fprintf(stderr, "failed to open file: %s\n", strerror(errno));
		exit(-1);
	}
  unsigned short opcode;
	while (2 ==read(fid, &opcode, 2) ) {
		printf("%04X ", htons(opcode));
		desc.describe(htons(opcode), stdout);
		printf("\n");
	}
	close(fid);
	exit(0);
}
