#ifndef LZ77_H
#define LZ77_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct token{
	int D;
	int L;
	char C;
	struct token* next;
}token;

token* Add_Token(int L, int D, char C);

token* LZ77_Compress(const char* input, unsigned long long input_length);

char* LZ77_Decompress(token* list_head, unsigned long long data_length);

unsigned long long Get_Data_Length(token* list_head);

void Delete_List(token* list_head);

#endif
