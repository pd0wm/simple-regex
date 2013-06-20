#include <stdio.h>

#include "regex.h"

int main(void){
	regex_generate_NFA_from_regex("a|b(cde)*");
	return 0;
}
