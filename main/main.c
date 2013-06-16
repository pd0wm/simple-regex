#include <stdio.h>

#include "regex.h"

int main(void){
	regex_generate_NFA_from_regex("ab(cde)*");
	return 0;
}
