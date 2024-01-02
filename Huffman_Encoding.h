#ifndef HUFFMAN_ENCODING_H
#define HUFFMAN_ENCODING_H

#include "LZ77.h"

typedef struct huffman_node{
	int symbol;
	int frequency;
	struct huffman_node* left;
	struct huffman_node* right;
}huffman_node;

typedef struct huffman_code{
	int symbol;
	int code_length;
	char* code;
}huffman_code;

typedef struct symbol_frequency{
	int symbol;
	int frequency;
}symbol_frequency;

symbol_frequency* Build_Frequency_Table(token* list_head, int* symbol_count);

huffman_node* Build_Huffman_Tree(symbol_frequency* frequency_table, int symbol_count);

huffman_code* Generate_Huffman_Codes(huffman_node* root, int symbol_count);

char* Encode_Data(token* list_head, huffman_code* h_codes, int symbol_count, unsigned long long* bit_count);

token* Decode_Huffman_Data(huffman_node* root, char* encoded_data, unsigned long long bit_count);

void Delete_Huffman_Tree(huffman_node* root);

void Delete_Huffman_Codes(huffman_code* h_codes, int symbol_count);

#endif
