#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>



int main (int argc, char * argv[]) {
	if (argc < 2) {
		fprintf(stderr, "requires arg\n");
		exit(-1);
	} 
	char line[1024];
		
	FILE *fid = fopen(argv[1],"r");

	if (fid <=0)  {
		fprintf(stderr, "failed to open file: %s\n", strerror(errno));
		exit(-1);
	}
  unsigned short opcode;
	while (!feof(fid) ) {
		if (NULL != fgets(line, sizeof(line), fid)) {
			opcode = ntohs(strtol(line, NULL, 16));	
			fwrite(&opcode, 2,1, stdout);
    }

	}
	fclose(fid);
	exit(0);
}
