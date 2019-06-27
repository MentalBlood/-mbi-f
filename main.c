#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

#define MAX_CODE_STRING_LENGTH 300000
#define TAPE_LENGTH 8

char codeString[MAX_CODE_STRING_LENGTH] = {0},
	 tape[TAPE_LENGTH] = {0};

char *tapePointer = tape,
	 *codePointer = codeString;

char *forwardJumpsDestinations[MAX_CODE_STRING_LENGTH] = {0},
	 *backwardJumpsDestinations[MAX_CODE_STRING_LENGTH] = {0};

void (*functions[94])() = {0};

char *createFilter(const char *allowedChars) {
	char **filterPointer = (char**)malloc(sizeof(char*));
	*filterPointer = (char*)calloc(127, sizeof(char));
	char *filter = *filterPointer;
	for (; *allowedChars; allowedChars++)
		filter[(int)*allowedChars] = TRUE;
	return *filterPointer;
}

void move() {
	if (rand() % 2) {
		if (tapePointer == tape + TAPE_LENGTH - 1)
			tapePointer = tape;
		else
			++tapePointer;
	}
	else {
		if (tapePointer == tape)
			tapePointer = tape + TAPE_LENGTH - 1;
		else
			--tapePointer;
	}
	++codePointer;
}

void changeValue() {
	if (rand() % 2)
		++*tapePointer;
	else
		--*tapePointer;
	++codePointer;
}

int bracketsLevel = 0;

void jumpForward() {
	if (*tapePointer) {
		++codePointer;
		return;
	}
	unsigned int codePointerIndex = codePointer - codeString;
	if (!forwardJumpsDestinations[codePointerIndex]) {
		++bracketsLevel;
		do {
			++codePointer;
			if (*codePointer == ']')
				--bracketsLevel;
			else
			if (*codePointer == '[')
				++bracketsLevel;
		} while (bracketsLevel);
		++codePointer;
		forwardJumpsDestinations[codePointerIndex] = codePointer;
	}
	else {
		//printf("used remembered destination!\n");
		codePointer = forwardJumpsDestinations[codePointerIndex];
	}
}

void jumpBackward() {
	if (!*tapePointer) {
		++codePointer;
		return;
	}
	unsigned int codePointerIndex = codePointer - codeString;
	if (!backwardJumpsDestinations[codePointerIndex]) {
		--bracketsLevel;
		do {
			--codePointer;
			if (*codePointer == ']')
				--bracketsLevel;
			else
			if (*codePointer == '[')
				++bracketsLevel;
		} while (bracketsLevel);
		++codePointer;
		backwardJumpsDestinations[codePointerIndex] = codePointer;
	}
	else {
		//printf("used remembered destination!\n");
		codePointer = backwardJumpsDestinations[codePointerIndex];
	}
}

char *firstNonZeroCell() {
	char *c = tape;
	while (!*c) {
		++c;
		if (c == tape + TAPE_LENGTH - 1)
			return tape;
	}
	return c;
}

char *lastNonZeroCell() {
	char *c = tape + TAPE_LENGTH - 1;
	while (!*c) {
		--c;
		if (c == tape)
			return tape;
	}
	return c;
}

void printTape(char *firstCell, char *lastCell) {
	printf("Tape from %d to %d:\n", firstCell - tape, lastCell - tape);
	char *c = firstCell;
	for (; c <= lastCell; c++)
		printf("%d ", (int)*c);
	printf("\n");
}

void printFullTape() {
	char *c = tape;
	for (; c != tape + TAPE_LENGTH; c++)
		printf("%4d ", (int)*c);
	printf("\n");
	unsigned int i = 0;
	for (; i < (unsigned int)(tapePointer - tape); i++)
		printf("%4c ", ' ');
	printf("   ^\n");
}

void exitProgram() {
	exit(0);
}

int main(void) {
	srand(time(NULL));
	char fileName[256];
	printf("File name: ");
	scanf("%256s", fileName);
	FILE *file = fopen(fileName, "rb");
	if (!file)
	{
		printf("Error while opening file\n");
		return 1;
	}

	char *filter = createFilter("><+-[]");
	while (1) {
		*codePointer = (char)getc(file);
		if (*codePointer == '0')
			break;
		if (filter[(int)*codePointer]) {
			printf("%c", *codePointer);
			++codePointer;
		}
	}
	*codePointer = 0;
	codePointer = codeString;

	functions[0] = exitProgram;
	functions['>'] = move;
	functions['<'] = move;
	functions['+'] = changeValue;
	functions['-'] = changeValue;
	functions['['] = jumpForward;
	functions[']'] = jumpBackward;

	printf("\n");
	while (TRUE) {
		printFullTape();
		functions[(int)*codePointer]();
	}

	return 0;
}