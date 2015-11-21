#include <iostream>
#include <stdlib.h>
using namespace std;


/*int strlen(char *str) {
	int i = 0;
	while(str[i]) {
		i++;
	}
	return i
} */

int main() {
	
	//char x[] = '12345678';
	//int count = strlen(x);
	char x[] = "abc";
	char *xptr = x;
	cout << *xptr++ << '\n';
	cout << *xptr++ << '\n';
	cout << *xptr++ << '\n';
	//char y[40];
	//itoa(x, y, 2);
	//x = x<<2;
	//cout << y;
	return 0;

}