#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <io.h>
#include <time.h>


#pragma comment(linker, "/STACK:300000000")
#pragma comment(linker, "/HEAP:300000000")

// Максимальное число символов, которое мы будем считывать из файла
#define MAX_FILE_LENGTH 250000000
// Максимальная длина предложения из конфигурационного файла, которое требуется удалить из текста
#define MAX_SENTENCE_LENGTH 2048

// Константа, показывающая, что требуется удалить все совпадения
#define ALL ULLONG_MAX

/* Некая смесь двух структур: сортированный однонаправленный список и стек.
В начало добавляются директивы с параметром reverse = false, в конец идут с reverse = true.
Затем с помощью функции pop() вынимаются элементы сверху стека, то есть сначала идут все директивы с reverse = true */
typedef struct stackNode{
	char* sentence;
	unsigned long long count;
	bool reverse;
	struct stackNode* ptr_next;
} sortedStackNode;

void insert(sortedStackNode** start_ptr, const char* string, size_t count, bool reverse);
sortedStackNode* pop(sortedStackNode** top_ptr);
sortedStackNode* getDirectivesFromConfigFile(FILE* configFile, sortedStackNode* startStackPtr);
char* deleteAllMatchesInText(sortedStackNode* startStackWithMathesPtr, char* text);

int main(int argc, char* argv[]) {
	if (argc != 3) return puts("Specify the configuration file and the text file");

	FILE* configFile = fopen(argv[1], "r+");
	FILE* textFile = fopen(argv[2], "r+");

	clock_t start = clock();
	// Открываем текстовый файл и считываем весь текст в строку
	char* text = (char*)calloc(MAX_FILE_LENGTH, sizeof(char));
	for (size_t i = 0; i < MAX_FILE_LENGTH; i++) {
		text[i] = fgetc(textFile);
		if (feof(textFile)) {
			text[i] = '\0';
			break;
		}
	}
	
	// Скидываем указатель назад на начало файла
	rewind(textFile);

	sortedStackNode* directivesStackTopPtr = NULL;
	directivesStackTopPtr = getDirectivesFromConfigFile(configFile, directivesStackTopPtr);
	// Получаем текст, из которого уже удалены все требуемые слова/предложения, указанные в конфигурационном файле
	char* textWithoutSentences = deleteAllMatchesInText(directivesStackTopPtr, text);
	// Очищаем основной файл
	_chsize(_fileno(textFile), 0);
	fputs(textWithoutSentences, textFile);

	clock_t stop = clock();
	printf("Execution time: %g seconds", ( (double) stop - (double) start) /(double) CLK_TCK);
}

sortedStackNode* getDirectivesFromConfigFile(FILE* configFile, sortedStackNode* startStackPtr) {
	char directive[MAX_SENTENCE_LENGTH];
	bool reverse = false;
	size_t count, numberFromString, i;

	while (!feof(configFile)) {
		// Читаем строку для удаления из файла
		fgets(directive, MAX_SENTENCE_LENGTH - 1, configFile);
		// Если последний символ строки считался как перенос строки, то удаляем перенос и ставим на его место нуль-терминатор
		char directiveLen = strlen(directive);
		char* lastSymbolPtr = &directive[directiveLen - 1];
		if (*lastSymbolPtr == '\n') *lastSymbolPtr = '\0';
		// Если первый символ команды - решетка, то это значит, что удалять требуется с конца
		reverse = directive[0] == '#' ? true : false;
		
		/* С помощью функции atoi получаем из директивы число (сколько раз нужно удалить указанное слово/команду).
		* Если числа нет, или не указан идентификатор (считать с начала или с конца), то считаем, что требуется удалить все совпадения.*/
		if (directive[0] == '#' || directive[0] == '^') { 
			numberFromString = atoi(&directive[1]);
			count = numberFromString ? numberFromString : 1;
		}
		else count = ALL;
		
		/* Вычленяем из директивы (команду) саму строку/слово для удаления, убирая первые i символов, на месте которых
		* стоят командные символы - # и ^ и дальше числа */
		for (i = 0; (isdigit(directive[i]) && i) || (i == 0 && (directive[0] == '#' || directive[0] == '^')); i++);
		char* currentLineToDelete = (char*)malloc((directiveLen + 1) * sizeof(char));
		strcpy(currentLineToDelete, &directive[i]);
		if (!strlen(currentLineToDelete)) continue;
		// Добавляем результат в наш стек-список
		insert(&startStackPtr, currentLineToDelete, count, reverse);
	}

	return startStackPtr;
}

char* deleteAllMatchesInText(sortedStackNode* startStackWithMathesPtr, char* text) {
	sortedStackNode* currentDirective = pop(&startStackWithMathesPtr);
	
	// Указатель на начало найденного совпадения части текста с удаляемым предложением/словом
	char* startMatchPtr;
	
	bool isTextReversed = false;
	while (currentDirective != NULL) {
		// Если надо удалять слова с конца, разворачиваем искомую строку, так как текст тоже развернут
		if (currentDirective->reverse) {
			// Если текст еще не развернут, то разворачиваем весь имеющийся на данный момент текст
			if (!isTextReversed) {
				text = _strrev(text);
				isTextReversed = true;
			}
	
			_strrev(currentDirective->sentence);
		}

		size_t searchedSentenceLength = strlen(currentDirective->sentence);
		// Ищем указатель на первый вход удаляемой подстроки в основную строку(текст)
		startMatchPtr = strstr(text, currentDirective->sentence);
		/* Идем по циклу, удаляя указанную в текущей директиве строку (слово) до тех пор, пока либо их не останется,
		либо не будет достигнуто требуемое число удалений*/
		for (unsigned long long i = 0; i < currentDirective->count && startMatchPtr != NULL; i++) {
			memmove(startMatchPtr, startMatchPtr + searchedSentenceLength, 1 + strlen(startMatchPtr + searchedSentenceLength));
			startMatchPtr = strstr(startMatchPtr, currentDirective->sentence);
		}
		
		// Очищаем память, выделенную под предыдущую директиву, и получаем из стека новую
		free(currentDirective->sentence);
		free(currentDirective);
		currentDirective = pop(&startStackWithMathesPtr);
	}

	// Если мы разворачивали текст, то вернем назад
	if (isTextReversed) text = _strrev(text);
	
	return text;
}

void insert(sortedStackNode** start_ptr, const char* string, size_t count, bool reverse) {
	sortedStackNode* new_ptr = NULL, * prev_ptr = NULL, * cur_ptr = *start_ptr;

	new_ptr = (sortedStackNode*)calloc(1, sizeof(sortedStackNode));

	if (new_ptr != NULL)
	{
		new_ptr->sentence = string;
		new_ptr->count = count;
		new_ptr->reverse = reverse;
		new_ptr->ptr_next = NULL;

		while (cur_ptr != NULL && reverse > cur_ptr->reverse)
		{
			prev_ptr = cur_ptr;
			cur_ptr = cur_ptr->ptr_next;
		}

		if (prev_ptr == NULL)
		{
			new_ptr->ptr_next = *start_ptr;
			*start_ptr = new_ptr;
		}
		else
		{
			prev_ptr->ptr_next = new_ptr;
			new_ptr->ptr_next = cur_ptr;
		}
	}
}

sortedStackNode* pop(sortedStackNode** top_ptr) {
	sortedStackNode* temp_ptr = NULL;

	if (*top_ptr == NULL)
	{
		return NULL;
	}

	temp_ptr = *top_ptr;
	*top_ptr = (*top_ptr)->ptr_next;

	return temp_ptr;
}