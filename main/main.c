#include <stdio.h>

#include "regex.h"

int main(void){
	FA_Graph NFA, DFA;
	printf("Create NFA -----------------\n");
	NFA = regex_generate_NFA_from_regex("a(b|c)*");
	printf("Create DFA -----------------\n");
	DFA = regex_generate_DFA_from_NFA(NFA);

	return 0;
}
