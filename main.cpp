#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

char *readSentence() {
	int bufSize = 64;
	char *buffer = (char*)malloc(bufSize * sizeof(char));
	assert(buffer != NULL); // This how we check allocation in debug mode. 
	char c;
	int i = 0;
	for (i; (c = getchar()) != '.' && c != EOF; i++) {
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
		free((void*)text[i]);
	}
	free((void*)text);
}


char** deleteBannedSentences(char **text, int *sz, int *banned) {
	//����� ������ �������, � ���� ��������� ������������ 3 ���� �� ���.
	//�� ��� �������� ������� ��� ���
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
			newText[newTextInd] = (char*)malloc(strlen(text[i]) * sizeof(char) + 1); // This (+ 1) cost me 1 hour of time.
			//If don't use it, the freeText() will throw a HEAP CORRUPTION DETECTED on debug x86/x64 and on release x86.
			//On release x64 it's all okay :=))).
			//Because strlen() of "hello" returns 5, but actually it is 6: do not forget about '\0' as 6`th char.
			strcpy(newText[newTextInd++], text[i]);
			//strcpy will write strlen(text[i]) + 1 
			//https://stackoverflow.com/questions/9551941/heap-corruption-detected-c
		}
	}

	freeText(text, *sz);
	free(banned);
	*sz = newSize;
	return newText;
}

void findEqualSentences(char ***text, int *sz) {
	/* _strcmpi(str1, str2) - ��������� ����� ��� ����� ��������. 
	   �� ���������� � ��������� ANSI C. ����� �� ������������?
	   ���� ���, �������� ������������ ��������� � �-� ������� tolower(int ch)*/

	int *banned = (int*)malloc(*sz * sizeof(int));
	for (int i = 0; i < *sz; i++) {
		banned[i] = 0;
	}

	for (int i = 0; i < *sz; i++) {
		for (int j = i + 1; j < *sz; j++) {
			if (_strcmpi((*text)[i], (*text)[j]) == 0) {
				banned[j] = 1; // ��� ���������� ������ ����� ����� �� ����� ������������ ��� �������� ������ ������
			}
		}
	}

	*text = deleteBannedSentences(*text, sz, banned);

}


int cmpChars(const void *a, const void *b) {
	//qsort comparator function for task 2
	return *(char*)a - *(char*)b;
}


void printCapitalsInAlphabetic(char *sent) {
	//������� 2)
	int len = strlen(sent);
	char *capitals = (char*)malloc(len * sizeof(char));
	int i, capInd = 0;
	for (i = 0; i < len; i++) {
		if (isupper(sent[i])) {
			capitals[capInd++] = sent[i];
		}
	}
	capitals = (char*)realloc(capitals, strlen(capitals) * sizeof(char));
	capitals[capInd] = '\0';
	qsort(capitals, strlen(capitals), sizeof(char), (int(*) (const void *, const void *)) cmpChars);
	printf("%s\n", capitals);
	free(capitals);
}


void solveTask2(char **text, int sz) {
	//2)��� ������� ����������� ������� ��� ��������� ����� � ������������������ �������.
	for (int i = 0; i < sz; i++) {
		printCapitalsInAlphabetic(text[i]);
	}
}


void solveTask4(char ***text, int *sz) {
	//4)������� ��� ����������� � ������� ��� �������� ����.
	int *banned = (int*)malloc(*sz * sizeof(int));
	for (int i = 0; i < *sz; i++) {
		banned[i] = 0;
	}

	int isLowers;
	int len;

	for (int i = 0; i < *sz; i++) {
		len = strlen((*text)[i]);
		isLowers = 0;
		for (int j = 0; j < len-1; j++) {//len - 1 ������ ��� �� ����� ��������� '.' .
			char c = (*text)[i][j];
			if (isalpha(c) && !isupper(c)) {
				isLowers = 1;
				break;
			}
		}
		if (isLowers == 0) {//������ ��� ��������
			banned[i] = 1;
		}
	}

	printf("Banned sentences: ");//DEGUB INFO
	for (int i = 0; i < *sz; i++) {
		printf("%i ", banned[i]);
	}
	printf("\n");

	*text = deleteBannedSentences(*text, sz, banned);

}


int calculateAverage(char *str) {
	//TODO_����_�_�����: ����� - �����, ��������� ������ �� ����. ����� tokenizer.
	//������ ��� ������� �� �����, ������� �� ����� ������ �� ����, ��� ������� ��� �����.
	int sum = 0;
	int cnt = 0, len = strlen(str);
	for (int i = 0; i < len - 1; i++) {
		if (isdigit(str[i])) {
			sum += atoi(&(str[i])); // ���� ������������ atof, �� ������� 1e2 ����������� ��� 100 ������ ��� ���������������� �������:=)
			//����� ������������ atoi, �� ����� �������� ��������
		//	printf("part sum %i\n", sum);
			cnt++;
		}
	}
	if (cnt > 0) {
	//	printf("%i %i\n", sum, cnt);
		return sum / cnt;
	}
	return 0;
}


int cmpByAverageSumOfNums(const void* s1, const void* s2) {
	//���������� ��� qsort �� 3 �������. ���������� ������ �� �������� ��������������� ����� � ���.
	char *str1 = *(char **)s1;
	char *str2 = *(char **)s2;
	int av1 = calculateAverage(str1);
	int av2 = calculateAverage(str2);
	return av1 < av2; // � ������� �������� �������� ���������������
	// ���� ���������� �� > �� � ������� �����������
}


void printText(char **text, int sz) {
	for (int i = 0; i < sz; i++) {
		printf("%s", text[i]);
	}
	printf("\n");
}


void solveTask3(char** text, int sz) {
	//3)������������� ����������� �� �������� ��������������� ����� � �����������. ����� - ����� ��������� ������ �� ����.
	qsort(text, sz, sizeof(char*), (int(*) (const void *, const void *)) cmpByAverageSumOfNums);
}


char **tokenize(char *str, int *sz) {
	//������� ����������� �� �����. ��������� ���� �����.
	int max = 10, maxWord = 10;
	char **tokens = (char**)malloc(max * sizeof(char*));
	int i = 0, tokensInd = 0;
	int j = 0;

	while (i < strlen(str)) {
		char *tmp = (char*)malloc(maxWord * sizeof(char));
		for (i; str[i] != ' ' && str[i] != ',' && str[i] != '.';i ++) {
			int x = str[i] == ' ' || str[i] == ',';
			tmp[j++] = str[i];
			if (j == maxWord - 3) {
				maxWord += 10;
				tmp = (char*)realloc(tmp, maxWord * sizeof(char));
			}
		}
		tmp[j] = '\0';
		tokens[tokensInd] = (char*)malloc(strlen(tmp) * sizeof(char) + 1);
		strcpy(tokens[tokensInd++], tmp);
		(*sz)++;
		if (tokensInd == max - 1) {
			max += 10;
			tokens = (char**)realloc(tokens, max * sizeof(char*));
		}
		j = 0;
		free(tmp);
		i++;
	}
	return tokens;
}


struct Token {
	char **token;
	int size;
	int *banned;
};


void printTokens(Token *tokens, int sz) { //DEBUG INFO
	for (int i = 0; i < sz; i++) {
		printf("===========================\n");
		for (int j = 0; j < tokens[i].size; j++) {
			printf("->%s", tokens[i].token[j]);
		}
		printf("\nBanned tokens: ");
		for (int j = 0; j < tokens[i].size; j++) {
			printf("%i ", tokens[i].banned[j]);
		}
		printf("\nNumber of tokens: %i\n", tokens[i].size);
		printf("===========================\n");
	}
}


void freeTokens(Token *tokens, int sz) {
	//�� ����� ������� ������ ��, ��� ���� �������� � ������� malloc/calloc/realloc
	for (int i = 0; i < sz; i++) {
		for (int j = 0; j < tokens[i].size; j++) {
			free(tokens[i].token[j]);
			//TODO_�����: free banned array - ����� ��������� ������ ��������� ����.
		}
	} free(tokens);
}


void findBannedWords(Token **tokens, int sz, char *bannedWord) {
	//����� �� ��������� �������, ����� ��� �������� �� ��������� ����� ������ ����-������ banned
	//�������� ������ ��� ���� ������ �� ������������� �������� - ���������� ������� � �����������.
	//����� ������������� ������.
	for (int i = 0; i < sz; i++) {
		int wordCountInToken = (*tokens)[i].size;
		for (int j = 0; j < wordCountInToken; j++) {
			(*tokens)[i].banned = (int*)malloc(sizeof(int) * wordCountInToken);
			for (int k = 0; k < wordCountInToken; k++) {
				(*tokens)[i].banned[k] = 0;
			}
		}
	}

	for (int i = 0; i < sz; i++) {
		int wordCountInToken = (*tokens)[i].size;
		for (int j = 0; j < wordCountInToken; j++) {
			if (strcmp((*tokens)[i].token[j], bannedWord) == 0) { // ����� ������ �����
				(*tokens)[i].banned[j] = 1;
			}
		}
	}
	//��� ��������))90))90))!!
}


void deleteBannedWords(Token **tokens, int sz) {
	for (int i = 0; i < sz; i++) {
		(*tokens)[i].token = deleteBannedSentences((*tokens)[i].token, &((*tokens)[i].size), (*tokens)[i].banned);
	}
	//��� ���� �����-�� ����� ��������))))))
}


void solveTask1(char** text, int sz) {
	//1)�� ���� ������ ������� ����� ��������� �������������. ���� ����� �������� � ����������� �� ��������� ����, ��� ���������� �������.
	struct Token *tokens = (Token *) malloc(sizeof(Token) * sz);
	for (int i = 0; i < sz; i++) {
		tokens[i].size = 0;
		tokens[i].token = tokenize(text[i], &tokens[i].size);
	}

	//TODO_C����: ������ �� ���� �����, ������� ����� �������
	char bannedWord[] = "Hello";
	
	findBannedWords(&tokens, sz, bannedWord);
	deleteBannedWords(&tokens, sz);

	printTokens(tokens, sz);
	freeTokens(tokens, sz);

}


int main() {
	FILE *in, *out;
	assert((in = freopen("in.txt", "r", stdin))!= NULL);//TODO_�����: ������ ������������� ����������� ������ �� ����������, 
	//������ ��� ��������� ���������� ����� � �������������. ��������, ��� ��� ��� ����������� (� ��� ��). 
	//� �����, ��� � ������� ������ ��������� ������� � �����������, � �� ����� ���������� ����� ��������� ����
	// "Deleted word "hello" in all text" Please type your next command >>
	//� ���������� � ����� � ������� ��� ��� ��������� �������������.
	assert((out = freopen("out.txt", "w", stdout))!=NULL);

	int sentencesCount = 0;
	char **text = readText(&sentencesCount);

	printText(text, sentencesCount);

	//findEqualSentences(&text, &sentencesCount);
	//solveTask2(text, sentencesCount);
	//solveTask4(&text, &sentencesCount);
	//printText(text, sentencesCount);
	//solveTask3(text, sentencesCount);
	//printText(text, sentencesCount);
	//solveTask1(text, sentencesCount);


	fclose(in);
	fclose(out);

	//int cmd;// ���������� ��������� ��������� ����������� ������
	/*while (1) { //TODO_�����: ����� ������� ���������� �����, ����� ������������ ������ ���� �������.
		printf("Type your command:");
		scanf("%i", &cmd);
		switch (cmd)
		{
		case 1:
			break;
		case 2: break;
		case 3: break;
		case 4: break; // �������� ����� = ���������
		case 5: exit(0);
		}
	}*/
}