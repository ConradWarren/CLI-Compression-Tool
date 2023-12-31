#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "LZ77.h"
#include "Huffman_Encoding.h"

//TODO: Removing Commas as seperations, first 3 byte will always be D, next byte will always
//be L, last byte will always be C.
//MUCH easier to Decompress, will decrease each token size by one byte, 
//average Huffman_code might be smaller (but there will be more zeros) 

//Done Edits to Build Frequency Table, still need to edit Encoding/Decoding
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

void Write_Huffman_Tree(FILE* file, huffman_node* root){
	
	if(root == NULL){
		fwrite(&((int){-2}),1, sizeof(int), file);
		return;
	}
	
	fwrite(&(root->symbol),sizeof(int),1, file);
	fwrite(&(root->frequency), sizeof(int),1, file);

	Write_Huffman_Tree(file, root->left);
	Write_Huffman_Tree(file, root->right);
}

int Deflate(const char* input_file_path, const char* output_file_path){
	
	FILE* file = fopen(input_file_path, "r");
	char* uncompressed_input = NULL;
	size_t uncompressed_data_length = 0;
	int symbol_count = 0;
	
	if(strcmp(input_file_path, output_file_path) == 0){
		fprintf(stderr, "Error: Input and Output Files Can Not be Equal\n");
		return EXIT_FAILURE;
	}

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
	unsigned long long encoded_data_length = strlen(encoded_data);	
	Delete_Huffman_Codes(h_codes, symbol_count);
	
	file = fopen(output_file_path, "wb");
	
	if(file == NULL){
		fprintf(stderr, "Error: Could Not Open Output File\n");
		free(encoded_data);
		free(uncompressed_input);
		Delete_Huffman_Tree(h_tree);
		return EXIT_FAILURE;
	}
	
	Write_Huffman_Tree(file, h_tree);
	fwrite(&encoded_data_length, sizeof(unsigned long long), 1, file);
	fwrite(encoded_data, 1, encoded_data_length, file);
	fclose(file);
	free(uncompressed_input);
	free(encoded_data);
	Delete_Huffman_Tree(h_tree);
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
	if(strcmp(argv[2], "inflate") == 0){
		
		if(argc != 4){
			fprintf(stderr, "Error: Input and Output File Not Reconized\n");
			return EXIT_FAILURE;
		}

	}

	fprintf(stderr,"Error: Could Not Reconize Command\n");
	return 0;
}
