#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct token{
	int D;
	int L;
	char C;
	struct token* next;
}token;

void Print_Token_List(token* head);

token* LZ77_Compress(const char* input, size_t input_length);

char* LZ77_Decompress(token* list_head);
