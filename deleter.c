#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc != 3) return puts("Specify the configuration file and the text file");

	FILE* configFile = fopen(argv[1], "r+");
	FILE* textFile = fopen(argv[2], "w+");
}