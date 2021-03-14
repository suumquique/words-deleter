#include <stdio.h>
#include <malloc.h>
#include <string.h>

#pragma comment(linker, "/STACK:2000000")
#pragma comment(linker, "/HEAP:6000000")

// ������������ ����� ��������, ������� �� ����� ��������� �� �����
#define MAX_FILE_LENGTH 5000000
// ������������ ����� ����������� �� ����������������� �����, ������� ��������� ������� �� ������
#define MAX_SENTENCE_LENGTH 2048
// ������������ ���������� ������ (�����������) � ���������������� �����
#define MAX_STRING_NUMBER 100

int main(int argc, char* argv[]) {
	if (argc != 3) return puts("Specify the configuration file and the text file");

	FILE* configFile = fopen(argv[1], "r+");
	FILE* textFile = fopen(argv[2], "r+");

	// ��������� ��������� ���� � ��������� ���� ����� � ������
	char* text = (char*)calloc(MAX_FILE_LENGTH, sizeof(char));
	for (size_t i = 0; i < MAX_FILE_LENGTH && !feof(textFile); i++) {
		text[i] = fgetc(textFile);
	}
	// ��������� ��������� ����� �� ������ �����
	rewind(textFile);

	char linesToDelete[MAX_STRING_NUMBER][MAX_SENTENCE_LENGTH];
	for (size_t i = 0; i < MAX_STRING_NUMBER && !feof(configFile); i++) {
		fgets(linesToDelete[i], MAX_SENTENCE_LENGTH - 1, configFile);
		char* lastSymbolPtr = &linesToDelete[i][strlen(linesToDelete[i]) - 1];
		if (*lastSymbolPtr == '\n') *lastSymbolPtr = '\0';
		puts(linesToDelete[i]);
	}
}