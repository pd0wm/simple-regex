#include <stdio.h>

#include "regex.h"

int main(void){
	printf("Create NFA -----------------\n");
	FA_Graph NFA = regex_generate_NFA_from_regex("a(b|c)*");
	printf("Create DFA -----------------\n");
	FA_Graph DFA = regex_generate_DFA_from_NFA(NFA);

	return 0;
}
