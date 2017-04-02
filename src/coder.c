#include <stdio.h>
#include <stdlib.h>
#include "coder.h"
#include <inttypes.h>

int encode(uint32_t code_point, CodeUnits *code_units)
{
	uint32_t count = 0;
	uint32_t i = code_point;
	while (i > 0) {
		i = i >> 1;
		count++;
	}
	//printf("%" "x" "\n", i);
	printf("%d\n", count);
	if (count < 8) {
		code_units->code[0] = code_point & 0x7F;
		code_units->length = 1;
		return 0;
	} else if (count < 12) {
		code_units->code[1] = 0x80 | (code_point & 0x3F);
		code_point = code_point >> 6;
		code_units->code[0] = 0xC0 | (code_point & 0x1F);
		code_units->length = 2;
		return 0;
	} else if (count < 17) {
		code_units->code[2] = 0x80 | (code_point & 0x3F);
		code_point = code_point >> 6;
		code_units->code[1] = 0x80 | (code_point & 0x3F);
		code_point = code_point >> 6;
		code_units->code[0] = 0xE0 | (code_point & 0xF);
		code_units->length = 3;
		return 0;
	} else if (count < 22) {
		code_units->code[3] = 0x80 | (code_point & 0x3F);
		code_point = code_point >> 6;
		code_units->code[2] = 0x80 | (code_point & 0x3F);
		code_point = code_point >> 6;
		code_units->code[1] = 0x80 | (code_point & 0x3F);
		code_point = code_point >> 6;
		code_units->code[0] = 0xF0 | (code_point & 0x7);
		code_units->length = 4;
		return 0;
	}
	return -1;
}

uint32_t decode(const CodeUnits *code_units)
{
	if ((code_units->code[0] >> 7) == 0) {
		return (code_units->code[0]);
	} else if (code_units->code[0] <= 0xDF) {
			return ((((code_units->code[0] & 0x1F) << 6) | (code_units->code[1] & 0x3F)));
	} else if (code_units->code[0] <= 0xEF) {
			return (((code_units->code[0] & 0xF) << 12) | ((code_units->code[1] & 0x3F) << 6) | (code_units->code[2] & 0x3F));
	} else if (code_units->code[0] <= 0xF7) {
			return (((code_units->code[0] & 0x7) << 18) | ((code_units->code[1] & 0x3F) << 12) | ((code_units->code[2] & 0x3F) << 6) | (code_units->code[3] & 0x3F));
	}	
	return -1;
}

int read_next_code_unit(FILE *in, CodeUnits *code_units, int *place)
{	
	uint8_t buf[4];
	int i = 0;
	code_zero(code_units);
	while (i == 0) {
		fread(&buf[i], 1, 1,in);
		*place = *place + 1;
		if ((buf[i] >> 6) == 0x2) {
			code_zero(code_units);
			return 0;
		}
		else if (buf[i] <= 0x7F) {
			code_units->code[0] = buf[0];
			code_units->length = 1;
			return 0;
		}
		else if (buf[i] <= 0xDF) {
			for (int j = i + 1; j < 2; j++) {
				fread(&buf[j], 1, 1,in);
				*place = *place + 1;
			}
			if (buf[1] <= 0xBF) {
				code_units->code[1] = buf[1];
				code_units->code[0] = buf[0];
				code_units->length = 2;
				return 0;
			}
			else { 
				*place = *place - 1;
				fseek(in, *place, SEEK_SET);
				code_zero(code_units);
				return 0;
			}
		}
		else if (buf[i] <= 0xEF) {
			for (int j = i + 1; j < 3; j++) {
				fread(&buf[j], 1, 1,in);
				*place = *place + 1;
			}
			if ((buf[1] <= 0xBF) & (buf[2] <= 0xBF)) {
				code_units->code[2] = buf[2];
				code_units->code[1] = buf[1];
				code_units->code[0] = buf[0];
				code_units->length = 3;
				return 0;
			}
			else { 
				*place = *place - 2;
				fseek(in, *place, SEEK_SET);
				code_zero(code_units);
				return 0;
			}
			
		}
		else if (buf[i] <= 0xF7) {
			for (int j = i + 1; j < 4; j++) {
				fread(&buf[j], 1, 1,in);
				*place = *place + 1;
			}
			if ((buf[1] <= 0xBF) & (buf[2] <= 0xBF) & (buf[3] <= 0xBF)) {
				code_units->code[3] = buf[3];
				code_units->code[2] = buf[2];
				code_units->code[1] = buf[1];
				code_units->code[0] = buf[0];
				code_units->length = 4;
				return 0;
			}
			else { 
				*place = *place - 3;
				fseek(in, *place, SEEK_SET);
				code_zero(code_units);
				return 0;
			}
			
		}
	i = -1;
	}
	return -1;
}

void write_code_unit(FILE *out, const CodeUnits *code_units)
{	
	fwrite(code_units->code, 1, code_units->length, out);
}

void code_zero(CodeUnits *code_units) {
	for (int i = 0; i < 4; i++) {
		code_units->code[i] = 0;
	}
}	
