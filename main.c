#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "LZ77.h"
#include "Huffman_Encoding.h"

//TODO: Change compressed data from bit string to indivisual bits
// Implement Inflate 

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

int Deflate(const char* input_file_path, const char* output_file_path){
	
	if(strcmp(input_file_path, output_file_path) == 0){
		fprintf(stderr, "Error: Input and Output Files Can Not be Equal\n");
		return EXIT_FAILURE;
	}
	
	FILE* file = fopen(input_file_path, "r");
	char* uncompressed_input = NULL;
	char* encoded_data = NULL;
	unsigned long long uncompressed_data_length = 0;
	unsigned long long encoded_data_length = 0;
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

	unsigned long long data_read = fread(uncompressed_input, 1, uncompressed_data_length, file);

	if(data_read != uncompressed_data_length){
		fprintf(stderr, "Error: Unable to Load Input FIle Into Memory\n");
		free(uncompressed_input);
		return EXIT_FAILURE;
	}

	uncompressed_input[uncompressed_data_length] = '\0';
	fclose(file);
	
	token* LZ77_token_list = LZ77_Compress(uncompressed_input, uncompressed_data_length);	
	free(uncompressed_input);

	symbol_frequency* frequency_table = Build_Frequency_Table(LZ77_token_list, &symbol_count);
	huffman_node* h_tree = Build_Huffman_Tree(frequency_table, symbol_count);
	free(frequency_table);
	huffman_code* h_codes = Generate_Huffman_Codes(h_tree, symbol_count);
	
	encoded_data = Encode_Data(LZ77_token_list, h_codes, symbol_count, &encoded_data_length);
	Delete_Huffman_Codes(h_codes, symbol_count);
	
	file = fopen(output_file_path, "wb");
	
	if(file == NULL){
		fprintf(stderr, "Error: Could Not Open Output File\n");
		free(encoded_data);
		Delete_Huffman_Tree(h_tree);
		Delete_List(LZ77_token_list);
		return EXIT_FAILURE;
	}
	
	Write_Huffman_Tree(file, h_tree);
	fwrite(&encoded_data_length, sizeof(unsigned long long), 1, file);
	fwrite(encoded_data, 1, (encoded_data_length%8 == 0) ? encoded_data_length/8 : encoded_data_length/8 + 1, file);
	fclose(file);

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
	unsigned long long encoded_bit_count = 0;
	char* encoded_data = NULL;
	char* decompressed_data = NULL;
	huffman_node* h_tree = NULL;
	token* LZ77_token_list = NULL;

	if(file == NULL){
		fprintf(stderr, "Error: Could Not Open Input File\n");
		return EXIT_FAILURE;
	}
	
	h_tree = Read_Huffman_Tree(file);
	fread(&encoded_bit_count, sizeof(unsigned long long), 1, file);
	encoded_data_length = (encoded_bit_count % 8 == 0) ? encoded_bit_count/8 : encoded_bit_count/8 + 1;
	encoded_data = (char*)malloc((encoded_data_length + 1) * sizeof(char));
	
	if(encoded_data == NULL){
		fprintf(stderr, "Error: Unable to Load Input File Into Memory\n");
		Delete_Huffman_Tree(h_tree);
		fclose(file);
		return EXIT_FAILURE;
	}
	
	unsigned long long bytes_read = fread(encoded_data, sizeof(char), encoded_data_length, file);
	fclose(file);
	if(bytes_read != encoded_data_length){
		fprintf(stderr, "Error: Input File Not Formated Correctly\n");
		Delete_Huffman_Tree(h_tree);
		free(encoded_data);
		return EXIT_FAILURE;
	}

	encoded_data[encoded_data_length] = '\0';
	LZ77_token_list = Decode_Huffman_Data(h_tree, encoded_data, encoded_bit_count); 
	Delete_Huffman_Tree(h_tree);
	free(encoded_data);

	unsigned long long decompressed_data_length = Get_Data_Length(LZ77_token_list);
	decompressed_data = LZ77_Decompress(LZ77_token_list, decompressed_data_length);
	
	if(decompressed_data == NULL){
		fprintf(stderr, "Error: Could not decompress data\n");
		return EXIT_FAILURE;
	}

	Delete_List(LZ77_token_list);

	file = fopen(output_file_path, "wb");
	if(file == NULL){
		fprintf(stderr, "Error: Could Not Open Output File");
		free(decompressed_data);
		return EXIT_FAILURE;
	}
	
	fwrite(decompressed_data, sizeof(char), decompressed_data_length, file);	
	fclose(file);
	free(decompressed_data);
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

