#include <stdio.h>
#include <malloc.h>
#include <string.h>

#pragma comment(linker, "/STACK:2000000")
#pragma comment(linker, "/HEAP:5000000")

#define MAX_FILE_LENGTH 2500000
#define READ_MAX_LEN 1024

int main(int argc, char* argv[]) {
	if (argc != 3) return puts("Specify the configuration file and the text file");

	FILE* configFile = fopen(argv[1], "r+");
	FILE* textFile = fopen(argv[2], "r+");

	char* text = (char*)calloc(MAX_FILE_LENGTH, sizeof(char));
	for (size_t i = 0; i < MAX_FILE_LENGTH && !feof(textFile); i++) {
		text[i] = fgetc(textFile);
	}
}