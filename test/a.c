#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct A {
	
	int num;
	int index[10];
	char mac[10][7];

};

void test(char *s) {
	
	printf("%c %c\n", s[0], s[1]);
	printf("strlen : %d\n", strlen(s));
	printf("%s\n", s);

	return;
}

int main() {

	char a[10][7];
	memset(a, 0, 70);
	a[0][0] = 'h';
	a[0][1] = 'e';
//	test(a[0]);

	char s[10];
	read(stdin, s, 10);
	printf("%s\n", s);

	return 0;
}
