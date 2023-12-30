#include "LZ77.h"

#define MAX_SEARCH_BUFFER_SIZE 256
#define MAX_LOOK_AHEAD_BUFFER_SIZE 256
#define MIN_PATTERN_MATCH_SIZE 0

token* Add_Token(int L, int D, char C){
	token* result = (token*)malloc(sizeof(token));
	result->L = L;
	result->D = D;
	result->C = C;
	result->next = NULL;
}

void Print_Token_List(token* head){
	
	if(head == NULL){
		return;
	}

	printf("(%d, %d, %c)\n", head->D, head->L, head->C);
	Print_Token_List(head->next);
}

token* LZ77_Compress(const char* input, size_t input_length){
	
	token* result = NULL;
	token* current = NULL;
	int best_match_distance = 0;
	int best_match_length = 0;

	for(long long i = 0; i<input_length; i += 1 + best_match_length){
		
		best_match_distance = 0;
		best_match_length = 0;

		for(long long j = i - 1; j >= 0 && i - j < MAX_SEARCH_BUFFER_SIZE; j--){
			
			if(input[i] != input[j]){
				continue;
			}

			for(long long z = 0; z + i + 1 < input_length && z < MAX_LOOK_AHEAD_BUFFER_SIZE; z++){
				
				if(input[j + z] != input[i + z]){
					break;
				}

				if(z + 1 > best_match_length && z + 1 > MIN_PATTERN_MATCH_SIZE){
					best_match_length = z + 1;
					best_match_distance = i - j;
				}		
			}	
		}
		
		if(result == NULL){
			result = Add_Token(best_match_length, best_match_distance, input[i + best_match_length]);
			current = result;
		}else{
			current->next = Add_Token(best_match_length, best_match_distance, input[i + best_match_length]);
			current = current->next;
		}

	}
	
	return result;
}

char* LZ77_Decompress(token* list_head){
	
	size_t string_capacity = 100;
	size_t string_size = 0;
	char* result = (char*)malloc((string_capacity + 1) * sizeof(char));
	
	while(list_head != NULL){
		while(string_size + list_head->L + 1 >= string_capacity){
			string_capacity += string_capacity / 2;
			result = (char*)realloc(result,(string_capacity + 1) * sizeof(char));
		}

	//	memcpy(result + string_size, result + string_size - list_head->D, list_head->L * sizeof(char));		
		//Opertunities for optimization here later. Can't do this straight out of the box as it can try to copy over itself

		for(int i = 0; i<list_head->L; i++){
			result[string_size + i] = result[string_size - list_head->D + i];
		}

		result[string_size + list_head->L] = list_head->C;
		string_size += list_head->L + 1;
		list_head = list_head->next;
	}
	result[string_size] = '\0';
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



