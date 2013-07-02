#include <stdio.h>

#include "regex.h"

int main(void){
	regex_match("a(b|c)*", "abbca dit is een test abbbc");


	printf("\n----------------\nPrint a key to exit\n");
	getchar();
	return 0;
}
