#include "LZ77.h"

#define MAX_SEARCH_BUFFER_SIZE 256
#define MAX_LOOK_AHEAD_BUFFER_SIZE 256
#define MIN_PATTERN_MATCH_SIZE 0

token* Add_Token(int L, int D, char C){
	token* result = (token*)malloc(sizeof(token));
	result->L = L;
	result->D = D;
	result->C = C;
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

	for(size_t i = 0; i<input_length; i += 1 + best_match_length){
		
		best_match_distance = 0;
		best_match_length = 0;

		for(size_t j = i - 1; j >= 0 && i - j < MAX_SEARCH_BUFFER_SIZE; j--){
			
			if(input[i] != input[j]){
				continue;
			}

			for(size_t z = 0; z + i < input_length && z < MAX_LOOK_AHEAD_BUFFER_SIZE; z++){
				
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
	
	Print_Token_List(result);

	return result;
}

char* LZ77_Decompress(token* list_head){

	return NULL;
}
