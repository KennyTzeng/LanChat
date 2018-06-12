#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct A {
	
	int num;
	int index[10];
	char mac[10][7];

};

void test(char s[]) {

	printf("%s\n", s);
	printf("%c\n", s[100]);

	return;
}

int main() {

	char a[10];
	memset(a, 0, 10);
	a[0] = 'h';
	a[1] = 'e';
	test(a);

	return 0;
}
