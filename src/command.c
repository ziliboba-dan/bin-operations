#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include "coder.h"
#include <inttypes.h>

int encode_file(const char *in_file_name, const char *out_file_name)
{
	FILE *in;
	in = fopen(in_file_name, "r");
	if (in == NULL) {
		return -1;
	}
	FILE *out;
	out = fopen(out_file_name, "wb");
	if (out == NULL) {
		return -1;
	}
	CodeUnits code_units;
	uint32_t code_point;
	while (fscanf(in,"%" SCNx32, &code_point) != EOF) {
		printf("> %" PRIx32 "\n", code_point);		
		if (encode(code_point, &code_units) != -1) {
			write_code_unit(out, &code_units);
		}
		else printf("Error in encode\n");
	}
	fclose(in);
	fclose(out);

	return 0;
}

int decode_file(const char *in_file_name, const char *out_file_name)
{
	FILE *in;
	in = fopen(in_file_name, "rb");
	if (in == NULL) {
		return -1;
	}
	CodeUnits code_units;
	FILE *out;
	out = fopen(out_file_name, "w");
	if (out == NULL) {
		return -1;
	}
	int place = 0;
	//uint32_t code_point;
	while (!read_next_code_unit(in, &code_units, &place)) {
		printf("Pointer = %d\n", place);
		printf("> %" PRIx32 "\n", decode(&code_units));	
		fprintf(out, "%" PRIx32, decode(&code_units));
		fprintf(out, "\n");
	}
	/*else {
		printf("Error in read_next_code_unit\n");
		return 0;
	}
	printf("Pointer = %d\n", place);
	printf("> %" PRIx32 "\n", decode(&code_units));	
	fprintf(out, "%" PRIx32, decode(&code_units));
	fprintf(out, "\n");
		*/
	fclose(in);
	fclose(out);
	return 0;
}
