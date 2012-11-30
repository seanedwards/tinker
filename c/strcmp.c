#include <stdio.h>
#include <string.h>

/* This file demonstrates an implementation of strcmp. You know, because I wanted to one day. */


int my_strcmp(const char* strA, const char* strB)
{
	while (*strA != '\0' && *strA == *strB) { ++strA; ++strB; }
	return *strA - *strB;
}

int my_strcmpi(const char* strA, const char* strB)
{
	int a = *strA | 32;
	int b = *strB | 32;
	while (a != '\0' && a == b) { a = *(++strA) | 32; b = *(++strB) | 32; }
	return a - b;
}

int main(int argc, const char** argv) {
	if (argc < 3) {
		printf("Please specify two arguments to compare.\n");
		return 1;
	}

	int cmp = my_strcmp(argv[1], argv[2]);
	int cmp2 = strcmp(argv[1], argv[2]);

	printf("Result strcmp : %d\n", cmp);
	if (cmp != cmp2)
		printf("Result does not match result of strcmp(%d)\n", cmp2);
	
	printf("Result strcmpi: %d\n", my_strcmpi(argv[1], argv[2]));
	return 0;
}

