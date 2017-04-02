#include <stdio.h>
#include "command.h"
#include "coder.h"
#include <string.h>

int main(int argc, char *argv[])
{
	char *NameFunc = argv[1];
	char *InFile = argv[2];
	char *OutFile = argv[3];
	int ptr1, ptr2;
	
	ptr1 = strncmp("encode", NameFunc, 10);
	ptr2 = strncmp("decode", NameFunc, 10);

	if ((argc != 4) || (((ptr1 == -1) || (ptr1 == 1)) & ((ptr2 == -1) || (ptr2 == 1)))) {
		printf("Usage:\n");
		printf("main encode <in-file-name> <out-file-name>\n");
		printf("main decode <in-file-name> <out-file-name>\n");
		return 0;
	}
	else if (ptr1 == 0) {
		printf("encode\n");
		encode_file(InFile, OutFile);	
	}
	else if (ptr2 == 0) {
		printf("decode\n");
		decode_file(InFile, OutFile);
	}
	return 0;
}
