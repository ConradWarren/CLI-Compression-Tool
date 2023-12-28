#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "LZ77.h"

//LZ77 compression -> Huffman Encoding -> Binary File.


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


	return 0;
}
