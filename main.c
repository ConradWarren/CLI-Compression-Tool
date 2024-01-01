#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "LZ77.h"
#include "Huffman_Encoding.h"

//TODO: Change compressed data from bit string to indivisual bits
// Implement Inflate 

void Print_Lists(token* list_1, token* list_2, int depth){

	if(list_1 == NULL && list_2 == NULL){
		printf("NULL   NULL\n");
		return;
	}

	if(list_1 == NULL){
		printf("%d : NULL   ", depth);
	}else{
		printf("%d : (%d, %d, %c) ",depth, list_1->D, list_1->L, list_1->C);
	}
	
	if(list_2 == NULL){
		printf("NULL\n");
	}else{
		printf("(%d, %d, %c)\n", list_2->D, list_2->L, list_2->C);
	}
	Print_Lists(list_1->next, list_2->next, depth+1);
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

huffman_node* Read_Huffman_Tree(FILE* file){
	
	int symbol = 0;
	int frequency = 0;
	
	fread(&symbol, sizeof(int), 1, file);

	if(symbol == -2) return NULL;

	fread(&frequency, sizeof(int), 1, file);
	
	huffman_node* node = (huffman_node*)malloc(sizeof(huffman_node));
	node->symbol = symbol;
	node->frequency = frequency;
	node->left = Read_Huffman_Tree(file);
	node->right = Read_Huffman_Tree(file);
	
	return node;
}

int Compare_Tree(huffman_node* h_tree1,huffman_node* h_tree2){
	
	if(h_tree1 == NULL && h_tree2 == NULL){
		return 1;
	}
	
	if(h_tree1 == NULL || h_tree2 == NULL){
		return 0;
	}
	
	if(h_tree1->symbol != h_tree2->symbol || h_tree1->frequency != h_tree2->frequency){
		return 0;
	}
	
	int left = Compare_Tree(h_tree1->left, h_tree2->left);
	int right = Compare_Tree(h_tree1->right, h_tree2->right);

	if(left == 1 && right == 1){
		return 1;
	}

	return 0;
}

int Compare_List(token* list_1, token* list_2, int depth){
	
	if(list_1 == NULL && list_2 == NULL){
		return 1;
	}

	if(list_1 == NULL || list_2 == NULL){
		printf("fails null test at %d\n", depth);
		return 0;
	}

	if(list_1->L != list_2->L || list_1->D != list_2->D || list_1->C != list_2->C){
		printf("fails at %d\n", depth);
		return 0;
	}

	return Compare_List(list_1->next, list_2->next, depth+1);
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
	char* test = LZ77_Decompress(LZ77_token_list);
	

	if(strcmp(test, uncompressed_input) == 0){
		printf("LZ77 decompression test passed\n");
	}else{
		printf("LZ77 decompression test failed\n");
	}

	free(test);

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
		Delete_List(LZ77_token_list);
		return EXIT_FAILURE;
	}
		
	Write_Huffman_Tree(file, h_tree);
	fwrite(&encoded_data_length, sizeof(unsigned long long), 1, file);
	fwrite(encoded_data, 1, encoded_data_length, file);
	fclose(file);

	file = fopen(output_file_path, "r");
	huffman_node* tree_2 = Read_Huffman_Tree(file);
	
	if(Compare_Tree(h_tree, h_tree) == 1){
		printf("tree test passed\n");
	}else{
		printf("tree test failed\n");
	}

	unsigned long long encoded_length2 = 0;
	fread(&encoded_length2, sizeof(unsigned long long), 1, file);
	printf("%lld\n", encoded_data_length);
	if(encoded_length2 == encoded_data_length){
		printf("Length test passed\n");
	}else{
		printf("Length test failed\n");
	}

	char* encoded_data_2 = (char*)malloc((encoded_length2 + 1) * sizeof(char));
	fread(encoded_data_2, sizeof(char), encoded_length2, file);
	encoded_data_2[encoded_length2] = '\0';
	
	if(strcmp(encoded_data_2, encoded_data) == 0){
		printf("string test passed\n");
	}else{
		printf("string test failed\n");
	}	
	
	token* token_list_2 = Decode_Huffman_Data(tree_2, encoded_data_2);
	
	if(Compare_List(LZ77_token_list, token_list_2, 0) == 1){
		printf("list test passed\n");
	}else{
		printf("list test failed\n");
		Print_Lists(LZ77_token_list, token_list_2, 0);
	}

	char* final_test = LZ77_Decompress(token_list_2);

	if(strcmp(final_test, uncompressed_input) == 0){
		printf("final test passed\n");
	}else{
		printf("final test failed\n");
	}

	free(uncompressed_input);
	free(encoded_data);
	Delete_Huffman_Tree(h_tree);
	Delete_List(LZ77_token_list);
	return 0;
}


int Inflate(const char* input_file_path, const char* output_file_path){
	
	if(strcmp(input_file_path, output_file_path) == 0){
		fprintf(stderr, "Error: Input and Output Files Can Not be Equal\n");
		return EXIT_FAILURE;
	}
	
	FILE* file = fopen(input_file_path, "r");
	unsigned long long encoded_data_length = 0;
	char* encoded_data = NULL;
	huffman_node* h_tree = NULL;
	token* LZ77_token_list = NULL;

	if(file == NULL){
		fprintf(stderr, "Error: Could Not Open Input File\n");
		return EXIT_FAILURE;
	}
	
	h_tree = Read_Huffman_Tree(file);
	fread(&encoded_data_length, sizeof(unsigned long long), 1, file);
	
	encoded_data = (char*)malloc((encoded_data_length+1) * sizeof(char));

	if(encoded_data == NULL){
		fprintf(stderr, "Error: Unable to Load Input File Into Memory\n");
		Delete_Huffman_Tree(h_tree);
		fclose(file);
		return EXIT_FAILURE;
	}
	

	fread(encoded_data, sizeof(char), encoded_data_length, file);
	encoded_data[encoded_data_length] = '\0';

	if(strlen(encoded_data) == encoded_data_length){
		printf("test passed\n");
	}else{
		printf("test failed\n");
	}
	
	LZ77_token_list = Decode_Huffman_Data(h_tree, encoded_data); 
	Print_Token_List(LZ77_token_list);	
	char* test = LZ77_Decompress(LZ77_token_list);
	
	for(int i = 0; i<h_tree->frequency; i++){
		printf("%c", test[i]);
	}
	printf("\n");
	Delete_List(LZ77_token_list);	
	Delete_Huffman_Tree(h_tree);
	fclose(file);
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
	if(strcmp(argv[1], "inflate") == 0){
		
		if(argc != 4){
			fprintf(stderr, "Error: Input and Output File Not Reconized\n");
			return EXIT_FAILURE;
		}
		return Inflate(argv[2], argv[3]);
	}

	fprintf(stderr,"Error: Could Not Reconize Command\n");
	return 0;
}
