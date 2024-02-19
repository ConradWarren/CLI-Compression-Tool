#include "LZ77.h"

#define MAX_SEARCH_BUFFER_SIZE 65535 //65535
#define MAX_LOOK_AHEAD_BUFFER_SIZE 255
#define MIN_PATTERN_MATCH_SIZE 0

token* Add_Token(int L, int D, char C){
	token* result = (token*)malloc(sizeof(token));
	result->L = L;
	result->D = D;
	result->C = C;
	result->next = NULL;
}

token* LZ77_Compress(const char* input, unsigned long long input_length){
	
	if(input_length == 0){
		return NULL;
	}

	token* result = Add_Token(0, 0, input[0]);
	token* current = result;
	int best_match_distance = 0;
	int best_match_length = 0;
	int prefix_function[MAX_LOOK_AHEAD_BUFFER_SIZE]; 
	int idx = 0;
	
	for(long long i = 1; i<input_length; i += 1 + best_match_length){
		
		best_match_distance = 0;
		best_match_length = 0;
		
		memset(prefix_function, 0, MAX_LOOK_AHEAD_BUFFER_SIZE * sizeof(int));
		idx = 0;
		for(int j = i + 1; j - i < MAX_LOOK_AHEAD_BUFFER_SIZE && j < input_length; j++){
			while(idx < 0 && input[j] != input[i + idx]){
				idx = prefix_function[idx - 1];
			}
			if(input[j] == input[i + idx]){
				prefix_function[j - i] = ++idx;
			}
		}
		idx = 0;
		for(long long j = (0 > i - MAX_SEARCH_BUFFER_SIZE) ? 0 : i - MAX_SEARCH_BUFFER_SIZE; j < i + MAX_LOOK_AHEAD_BUFFER_SIZE && j < input_length; j++){
			
			while(idx > 0 && input[i + idx] != input[j]){
				idx = prefix_function[idx-1];
			}
			
			if(input[j] == input[i + idx]){
				idx++;
			}

			if(j - idx + 1 >= i) break;
			
			if(idx > best_match_length && i + idx < input_length){
				best_match_length = idx;
				best_match_distance = i - (j - idx + 1);
			}

			if(idx == MAX_LOOK_AHEAD_BUFFER_SIZE) break;
		}
		
		current->next = Add_Token(best_match_length, best_match_distance, input[i + best_match_length]);
		current = current->next;
	}
	
	return result;
}

char* LZ77_Decompress(token* list_head, unsigned long long data_length){
	
	char* result = (char*)malloc((data_length + 1) * sizeof(char));
	unsigned long long string_size = 0;
	
	if(result == NULL){
		printf("Error: failed to allocate memory for decompression\n");
		return NULL;
	}

	while(list_head != NULL){
		
		for(unsigned long long i = 0; i<list_head->L; i++){
			result[string_size + i] = result[string_size - list_head->D + i];
		}

		result[string_size + list_head->L] = list_head->C;
		string_size += list_head->L + 1;
		list_head = list_head->next;
	}
	result[string_size] = '\0';
	return result;
}

unsigned long long Get_Data_Length(token* list_head){

	unsigned long long result = 0;

	while(list_head != NULL){
		result += list_head->L + 1;
		list_head = list_head->next;
	}

	return result;
}

void Delete_List(token* list_head){
	
	token* next = NULL;
	while(list_head != NULL){
		next = list_head->next;
		free(list_head);
		list_head = next;
	}
}



