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

	//TODO: frquency table can only handle L and D values of 8 bits, needs to increased
	//in the future
	
	while(list_head != NULL){
		frequency_table[(unsigned char)','].frequency += 3;
		frequency_table[(unsigned char)list_head->C].frequency++;
		frequency_table[(unsigned char)list_head->L].frequency++;
		frequency_table[(unsigned char)list_head->D].frequency++;
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
