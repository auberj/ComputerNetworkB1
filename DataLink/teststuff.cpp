#include <iostream>
using namespace std;


int strlen(char *str) {
	int i = 0;
	while(str[i]) {
		i++;
	}
	return i
} 

int main() {
	char x[] = '12345678';
	int count = strlen(x);
	cout << count;
	return 0;

}