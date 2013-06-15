#include <stdio.h>
#include <stdlib.h>

#include "regex.h"

/*
	This function generates a NFA state diagram for the given regex
*/
NFA_State *regex_generate_NFA_from_regex(char *regex) {
	// Print regex
	printf("Regex: %s\n", regex);

	// Alocate memory for first state
	NFA_State *start = (NFA_State*) malloc( sizeof(NFA_State) );

	// Process each character of the regex
	char *cur_char = regex;
	while (*cur_char != '\0') {
		printf("%c\n", *cur_char);

		cur_char++;
	}


	return start;
}
