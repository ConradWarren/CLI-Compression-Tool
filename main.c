#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "LZ77.h"
#include "Huffman_Encoding.h"

//TODO: Currently we can only handle a search buffer of one byte. Need to change many things about Huffman_Encoding
//to handle a search buffer of 1-8Mb bytes
//Things to change:
//	Frequency table calculations
//	Encoding Data
//	Decoding Data

int main(int argc, char** argv){
	
	if(argc < 2){
		printf("Error: No Argument Given\n");
		return EXIT_FAILURE;
	}

	printf("pre-compression: %s\n", argv[1]);
	
	token* LZ77_List = LZ77_Compress(argv[1], strlen(argv[1]));
	
	char* test = LZ77_Decompress(LZ77_List);

	printf("post-compression %s\n", test);

	if(strcmp(test, argv[1]) == 0){
		printf("test passed\n");
	}else{
		printf("test failed\n");
	}
	
	int symbol_count = 0;
	symbol_frequency* frequency_table = Build_Frequency_Table(LZ77_List, &symbol_count);	
	printf("symbol count = %d\n", symbol_count);	
	
	huffman_node* huffman_tree = Build_Huffman_Tree(frequency_table, symbol_count);

	huffman_code* h_codes = Generate_Huffman_Codes(huffman_tree, symbol_count);
	
	char* encoded_data = Encode_Data(LZ77_List, h_codes, symbol_count);	
	
	printf("Encoded_Data : %s\n", encoded_data);

	Decode_Huffman_Data(huffman_tree, encoded_data);

	return 0;
}
