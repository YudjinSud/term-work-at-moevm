#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

char *readSentence() {
	int bufSize = 64;
	char *buffer = (char*)malloc(bufSize * sizeof(char));
	assert(buffer != NULL); // How we check allocation in debug mode. 
	char c;
	int i = 0;
	for (i; (c = getchar()) != '.' && c != EOF; ++i) {
		if (i == bufSize - 3) { //sentence soon probably will be bigger than allocated
			bufSize += 10;
			buffer = (char*) realloc(buffer, bufSize * sizeof(char));
		}
		buffer[i] = c;
	}	
	buffer[i] = '.';
	buffer[i+1] = '\0';
	return buffer;
}


char **readText(int *sz) {
	int maxTextSize = 10;
	char **text = (char**)malloc(maxTextSize * sizeof(char*));
	char *str;
	while (strcmp((str = readSentence()), ".")) {
		if (*sz == maxTextSize - 1) {
			maxTextSize += 10;
			text = (char**)realloc(text, maxTextSize * sizeof(char*));
		}
		text[(*sz)++] = str;
	}
	return text;
}


void freeText(char **text, int sz) {
	for (int i = 0; i < sz; i++) {
		free(text[i]);
	}
	free(text);
}


char** deleteSameSentences(char **text, int *sz, int *banned) {
	int newSize = 0;
	for (int i = 0; i < *sz; i++) {
		if (banned[i] == 0) {
			newSize++;
		}
	}

	char **newText = (char**)malloc(newSize * sizeof(char*));

	int newTextInd = 0;
	for (int i = 0; i < *sz; i++) {
		if (banned[i] == 0) {
			newText[newTextInd] = (char*)malloc(strlen(text[i]) * sizeof(char));
			strcpy(newText[newTextInd++], text[i]);
		}
	}

	freeText(text, *sz);
	free(banned);
	*sz = newSize;
	return newText;

}

void findSameSentences(char ***text, int *sz) {
	/* _strcmpi(str1, str2) - —равнение строк без учета регистра. 
	   Ќе определено в стандарте ANSI C. ћожно ли использовать?
	   если нет, написать посимвольное сравнение с п-ю функции tolower(int ch)*/

	int *banned = (int*)malloc(*sz * sizeof(int));
	for (int i = 0; i < *sz; i++) {
		banned[i] = 0;
	}

	for (int i = 0; i < *sz; i++) {
		for (int j = i + 1; j < *sz; j++) {
			if (_strcmpi((*text)[i], (*text)[j]) == 0) {
				banned[j] = 1; // все одинаковые строки кроме одной не будут копироватьс€ при создании нового текста
			}
		}
	}

	*text = deleteSameSentences(*text, sz, banned);

}


int main() {
	FILE *in, *out;
	assert((in = freopen("in.txt", "r", stdin))!= NULL);
	assert((out = freopen("out.txt", "w", stdout))!=NULL);

	int sentencesCount = 0;
	char **text = readText(&sentencesCount);

	for (int i = 0; i < sentencesCount; i++) {
		printf("%s", text[i]);
	}
	printf("\n");

	findSameSentences(&text, &sentencesCount);

	for (int i = 0; i < sentencesCount; i++) {
		printf("%s", text[i]);
	}

	fclose(in);
	fclose(out);

}