#include "Huffman_Encoding.h"

typedef struct min_heap{
	huffman_node** arr;
	int heap_size;
	int heap_capacity;
}min_heap;

void Init_Heap(min_heap* _heap, int symbol_count){
	_heap->heap_capacity = symbol_count;
	_heap->heap_size = 0;
	_heap->arr = (huffman_node**)malloc(_heap->heap_capacity * sizeof(huffman_node*));
}

huffman_node* Pop_Heap(min_heap* _heap){
	
	huffman_node* result = _heap->arr[0];
	_heap->heap_size--;
	_heap->arr[0] = _heap->arr[_heap->heap_size];
	int idx = 0;
	int left_child_idx = idx * 2 + 1;
	int right_child_idx = idx * 2 + 2;
	
	while((left_child_idx < _heap->heap_size && _heap->arr[left_child_idx]->frequency < _heap->arr[idx]->frequency) ||(right_child_idx < _heap->heap_size && _heap->arr[right_child_idx]->frequency < _heap->arr[idx]->frequency)){
		
		if(right_child_idx < _heap->heap_size && _heap->arr[right_child_idx]->frequency < _heap->arr[left_child_idx]->frequency){
			huffman_node* temp = _heap->arr[idx];
			_heap->arr[idx] = _heap->arr[right_child_idx];
			_heap->arr[right_child_idx] = temp;
			idx = right_child_idx;

		}else{
			huffman_node* temp = _heap->arr[idx];
			_heap->arr[idx] = _heap->arr[left_child_idx];
			_heap->arr[left_child_idx] = temp;
			idx = left_child_idx;
		}
		left_child_idx = idx * 2 + 1;
		right_child_idx = idx * 2 + 2;
	}

	return result;
}

void Push_Heap(min_heap* _heap, huffman_node* node){
	
	int idx = _heap->heap_size;
	int parent_idx = (idx-1)/2;
	int left_child_idx = 0;
	int right_child_idx = 0;
	_heap->heap_size++;
	_heap->arr[idx] = node;

	while(idx > parent_idx && _heap->arr[idx]->frequency < _heap->arr[parent_idx]->frequency){
		huffman_node* temp = _heap->arr[idx];
		_heap->arr[idx] = _heap->arr[parent_idx];
		_heap->arr[parent_idx] = temp;
		idx = parent_idx;
		parent_idx = (idx - 1)/2;
	}
	left_child_idx = idx * 2 + 1;
	right_child_idx = idx * 2 + 2;
	
	while((left_child_idx < _heap->heap_size && _heap->arr[left_child_idx]->frequency < _heap->arr[idx]->frequency) ||(right_child_idx < _heap->heap_size && _heap->arr[right_child_idx]->frequency < _heap->arr[idx]->frequency)){
		
		if(right_child_idx < _heap->heap_size && _heap->arr[right_child_idx]->frequency < _heap->arr[left_child_idx]->frequency){
			huffman_node* temp = _heap->arr[idx];
			_heap->arr[idx] = _heap->arr[right_child_idx];
			_heap->arr[right_child_idx] = temp;
			idx = right_child_idx;

		}else{
			huffman_node* temp = _heap->arr[idx];
			_heap->arr[idx] = _heap->arr[left_child_idx];
			_heap->arr[left_child_idx] = temp;
			idx = left_child_idx;
		}
		left_child_idx = idx * 2 + 1;
		right_child_idx = idx * 2 + 2;
	}
}

huffman_node* Build_Huffman_Tree(symbol_frequency* frequency_table, int symbol_count){
	
	int idx = 0;
	min_heap _heap;

	Init_Heap(&_heap, symbol_count);
		
	for(int i = 0; i<256; i++){
		
		if(frequency_table[i].frequency == 0){
			continue;
		}
		
		huffman_node* temp = (huffman_node*)malloc(sizeof(huffman_node));
		temp->symbol = frequency_table[i].symbol;
		temp->frequency = frequency_table[i].frequency;
		temp->right = NULL;
		temp->left = NULL;
		Push_Heap(&_heap, temp);
	}
	
	huffman_node* internal_node = NULL;
	while(_heap.heap_size > 1){
		internal_node = (huffman_node*)malloc(sizeof(huffman_node));
		internal_node->symbol = -1;
		internal_node->left = Pop_Heap(&_heap);
		internal_node->right = Pop_Heap(&_heap);
		internal_node->frequency = internal_node->left->frequency + internal_node->right->frequency;
		Push_Heap(&_heap, internal_node);
	}
	free(_heap.arr);
	return internal_node;
}

symbol_frequency* Build_Frequency_Table(token* list_head, int* symbol_count){
	
	
	symbol_frequency* frequency_table = (symbol_frequency*)malloc(256 * sizeof(symbol_frequency));
	
	for(int i = 0; i<256; i++){
		frequency_table[i].symbol = i;
		frequency_table[i].frequency = 0;
	}
	
	while(list_head != NULL){
		frequency_table[(unsigned char)list_head->C].frequency++;
		frequency_table[(unsigned char)list_head->L].frequency++;
		frequency_table[(unsigned char)(list_head->D & 0xFF)].frequency++;
		frequency_table[(unsigned char)((list_head->D >> 8) & 0xFF)].frequency++;
		frequency_table[(unsigned char)((list_head->D >> 16) & 0xFF)].frequency++;	
		list_head = list_head->next;
	}
	
	for(int i = 0; i<256; i++){
		if(frequency_table[i].frequency != 0) (*symbol_count)++;
	}

	return frequency_table;
}


void Generate_Huffman_Codes_Recursive(huffman_node* root, huffman_code* h_codes, int depth, char* code, int* idx){
	
	if(root->symbol != -1){
		h_codes[*idx].symbol = root->symbol;
		h_codes[*idx].code_length = depth;
		h_codes[*idx].code = (char*)malloc((depth+1)*sizeof(char));
		memcpy(h_codes[*idx].code, code, depth+1);
		(*idx)++;
		return;
	}
	
	code[depth] = '0';
	code[depth+1] = '\0';
	Generate_Huffman_Codes_Recursive(root->left, h_codes, depth+1, code, idx);

	code[depth] = '1';
	code[depth+1] = '\0';
	Generate_Huffman_Codes_Recursive(root->right, h_codes, depth+1, code, idx);
}

huffman_code* Generate_Huffman_Codes(huffman_node* root, int symbol_count){
	huffman_code* h_codes = (huffman_code*)malloc(symbol_count * sizeof(huffman_code));
	int idx = 0;
	char codes[257];
	
	Generate_Huffman_Codes_Recursive(root, h_codes, 0, codes, &idx);

	return h_codes;
}

char* Encode_Data(token* list_head, huffman_code* h_codes, int symbol_count, unsigned long long* bit_count){
	
	char* encoded_data = NULL;
	unsigned long long encoded_length = 0;
	long long idx = 0;
	char** table = (char**)malloc(256 * sizeof(char*));
	int* code_lengths = (int*)malloc(256 * sizeof(int));
	
	token* current = list_head;

	for(int i = 0; i<symbol_count; i++){
		table[(unsigned char)h_codes[i].symbol] = (char*)malloc((h_codes[i].code_length+1) * sizeof(char));
		memcpy(table[(unsigned char)h_codes[i].symbol], h_codes[i].code, (h_codes[i].code_length + 1) * sizeof(char));
		code_lengths[h_codes[i].symbol] = h_codes[i].code_length;
	}
	
	while(current != NULL){
		encoded_length += code_lengths[(unsigned char)current->L];
		encoded_length += code_lengths[current->D & 0xFF];
		encoded_length += code_lengths[(current->D >> 8) & 0xFF];
		encoded_length += code_lengths[(current->D >> 16) & 0xFF];
		encoded_length += code_lengths[(unsigned char)current->C];	
		current = current->next;
	}
	
	encoded_data = (char*)malloc((encoded_length + 1) * sizeof(char));
	encoded_data[encoded_length] = '\0';

	while(list_head != NULL){
		memcpy(encoded_data + idx, table[(unsigned char)((list_head->D >> 16) & 0xff)], code_lengths[(unsigned char)((list_head->D >> 16) & 0xff)] * sizeof(char));
		idx += code_lengths[(unsigned char)((list_head->D >> 16) & 0xff)];
		memcpy(encoded_data + idx, table[(unsigned char)((list_head->D >> 8) & 0xff)], code_lengths[(unsigned char)((list_head->D >> 8) & 0xff)] * sizeof(char));
		idx += code_lengths[(unsigned char)((list_head->D >> 8) & 0xff)];		
		memcpy(encoded_data + idx, table[(unsigned char)(list_head->D & 0xff)], code_lengths[(unsigned char)(list_head->D & 0xff)] * sizeof(char));
		idx += code_lengths[(unsigned char)(list_head->D & 0xff)];
		memcpy(encoded_data + idx, table[(unsigned char)list_head->L], code_lengths[(unsigned char)list_head->L] * sizeof(char));
		idx += code_lengths[(unsigned char)list_head->L];
		memcpy(encoded_data + idx, table[(unsigned char)list_head->C], code_lengths[(unsigned char)list_head->C] * sizeof(char));
		idx += code_lengths[(unsigned char)list_head->C];
		list_head = list_head->next;
	}

	(*bit_count) = encoded_length;

	unsigned long long bitwise_encoded_length = (encoded_length % 8 == 0) ? encoded_length / 8 : encoded_length / 8 + 1;
	char* bit_wise_encoded_data = (char*)calloc(bitwise_encoded_length + 1,sizeof(char));

	for(int i = 0; i<encoded_length; i++){
		if(encoded_data[i] == '1'){
			bit_wise_encoded_data[i/8] |= (1 << (i%8));
		}
	}

	for(int i = 0; i<symbol_count; i++){
		free(table[(unsigned char)h_codes[i].symbol]);
	}

	free(table);
	free(code_lengths);
	free(encoded_data);
	
	return bit_wise_encoded_data;
}

token* Decode_Huffman_Data(huffman_node* root, char* encoded_data, unsigned long long bit_count){
	
	huffman_node* current = root;
	token* result = NULL;
	token* current_token = NULL;
	unsigned char* str_buffer = (unsigned char*)malloc((bit_count + 1) * sizeof(char));
	int str_buffer_idx = 0;
	
	for(int i = 0; i < bit_count; i++){
		
		current = (encoded_data[i/8] & (1 << (i % 8))) ? current->right : current->left;
		
		if(current->symbol != -1){
			str_buffer[str_buffer_idx] = (unsigned char)current->symbol;
			str_buffer_idx++;
			current = root;
		}
	}

	for(int i = 0; i<str_buffer_idx; i += 5){
		
		if(result == NULL){
			result = Add_Token((unsigned char)str_buffer[i+3],(((int)str_buffer[i]) << 16) | (((int)str_buffer[i+1]) << 8) | (((int)str_buffer[i+2])),(char)str_buffer[i+4]);
			current_token = result;
		}else{
			current_token->next = Add_Token((unsigned char)str_buffer[i+3],(((int)str_buffer[i]) << 16) | (((int)str_buffer[i+1]) << 8) | (((int)str_buffer[i+2])),(char)str_buffer[i+4]);
			current_token = current_token->next;
		}
	}
	free(str_buffer);
	return result;
}

void Delete_Huffman_Tree(huffman_node* root){
	
	if(root == NULL){
		return;
	}
	
	Delete_Huffman_Tree(root->left);
	Delete_Huffman_Tree(root->right);
	free(root);
}

void Delete_Huffman_Codes(huffman_code* h_codes, int symbol_count){
	
	for(int i = 0; i<symbol_count; i++){
		free(h_codes[i].code);
	}
	free(h_codes);
}
