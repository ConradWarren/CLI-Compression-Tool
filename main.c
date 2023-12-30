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

void Compare_Lists(token* list_1, token* list_2){
	
	if(list_1 == NULL && list_2 == NULL){
		return;
	}

	if(list_1 != NULL){
		printf("(%d, %d, %c)  ", list_1->D, list_1->L, list_1->C);
		list_1 = list_1->next;
	}else{
		printf("NULL  ");
	}
	if(list_2 != NULL){
		printf("(%d, %d, %c)\n", list_2->D, list_2->L, list_2->C);
		list_2 = list_2->next;
	}else{
		printf("NULL\n");
	}

	Compare_Lists(list_1, list_2);
}

int Deflate(const char* input_file_path, const char* output_file_path){
	
	FILE* file = fopen(input_file_path, "r");
	char* uncompressed_input = NULL;
	size_t uncompressed_data_length = 0;
	int symbol_count = 0;
	
	if(file == NULL){
		fprintf(stderr, "Error: Could Not Open Input File\n");
		return EXIT_FAILURE;
	}

	fseek(file, 0, SEEK_END);
	uncompressed_data_length = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	uncompressed_input = (char*)malloc((uncompressed_data_length+1) * sizeof(char));
	
	if(uncompressed_input == NULL){
		fprintf(stderr, "Error: Unable to Load Input File Into Memory\n");
		return EXIT_FAILURE;
	}
	
	fread(uncompressed_input, 1, uncompressed_data_length, file);
	uncompressed_input[uncompressed_data_length] = '\0';
	fclose(file);
	
	token* LZ77_token_list = LZ77_Compress(uncompressed_input, uncompressed_data_length);
	
	symbol_frequency* frequency_table = Build_Frequency_Table(LZ77_token_list, &symbol_count);
	huffman_node* h_tree = Build_Huffman_Tree(frequency_table, symbol_count);
	free(frequency_table);
	huffman_code* h_codes = Generate_Huffman_Codes(h_tree, symbol_count);

	char* encoded_data = Encode_Data(LZ77_token_list, h_codes, symbol_count);
	Delete_Huffman_Codes(h_codes, symbol_count);
	
	token* test = Decode_Huffman_Data(h_tree, encoded_data); 
	char* test_output = LZ77_Decompress(test);
	
	Compare_Lists(LZ77_token_list, test);	

	return 0;
	if(strcmp(test_output, uncompressed_input) == 0){
		printf("test passed\n");
	}else{
		printf("test failed, test output :\n%s\n", test_output);
	}

	return 0;
}


int main(int argc, char** argv){
	
	if(argc < 2){
		fprintf(stderr, "Error: No Arguments Given\n");
		return EXIT_FAILURE;
	}
	
	if(strcmp(argv[1], "deflate") == 0){
		
		if(argc != 4){
			fprintf(stderr, "Error: Input and Output File Not Reconized\n");
			return EXIT_FAILURE;
		}
		
		return Deflate(argv[2], argv[3]);
	}

	fprintf(stderr,"Error: Could Not Reconize Command\n");
	return 0;
}
