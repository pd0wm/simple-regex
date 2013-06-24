#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "regex-helper.h"
#include "nfa.h"


/*
This function generates a NFA graph for the given regex
*/
FA_Graph regex_generate_NFA_from_regex(char *regex) {
	FA_Graph graph;
	FA_State *start_state,*cur_state;
	char *cur_char;
	// Print regex
	printf("Regex: %s\n", regex);

	// Alocate memory for first state
	start_state = regex_create_empty_FA_state();
	cur_state = start_state;

	// Process each character of the regex
	cur_char = regex;
	while (*cur_char != '\0') {
		printf("%c\n", *cur_char);

		// Check if cur_char is not a special character
		if ( isalnum( (int)(*cur_char) ) ) {
			// Normal charater check next character
			char *next_char = cur_char + 1;
			if (*next_char != '*' && *next_char != '+'){
				// Next character is a normal character or group start or group end
				// So just create a next state
				//    a
				// O------>O
				FA_State *next_state =  regex_create_empty_FA_state();
				next_state->transitions = NULL;
				next_state->end = 0;
				regex_link_NFA_states(cur_state,next_state,*cur_char);

				// Move state pointer to next state
				cur_state = next_state;

				// If it is a group end, we should return
				if (*next_char == ')'){
					FA_Graph graph;
					graph.begin = start_state;
					graph.end = cur_state;

					printf("returning after )\n");
					return graph;
				}
			}else if(*next_char == '*'){
				// Next character is a *
				// This matches zero or more instances of the current character

				// Allocate memory for new states and link
				FA_State *group_begin = regex_create_empty_FA_state();
				FA_State *group_end = regex_create_empty_FA_state();
				regex_link_NFA_states(group_begin,group_begin,*cur_char);

				cur_state = regex_link_zero_or_more(cur_state, group_begin, group_end);

				//Skip next character
				cur_char++;
			}else if(*next_char == '+'){
				// Next character is a +
				// This matches one or more instances of the current character

				// Allocate memory for new states and link
				FA_State *group_begin = regex_create_empty_FA_state();
				FA_State *group_end = regex_create_empty_FA_state();
				regex_link_NFA_states(group_begin,group_begin,*cur_char);

				cur_state = regex_link_one_or_more(cur_state, group_begin, group_end);

				//Skip next character
				cur_char++;
			}

		} else if (*cur_char == '('){
			FA_Graph group_graph;
			FA_State *group_begin,*group_end;
			char *next_char;

			// Get substring for group and call this function recursively on the substring
			char *substr = regex_get_group(cur_char);

			// Move cur_char to end of group
			cur_char += strlen(substr) + 1;

			// Get begin and end of group
			group_graph = regex_generate_NFA_from_regex(substr);
			group_begin = group_graph.begin;
			group_end = group_graph.end;

			// Free memory for substr
			free(substr);
			substr = NULL;

			next_char = cur_char + 1;

			if(*next_char == '*'){
				// Next character is a *
				// This matches zero or more instances of the current character
				cur_state = regex_link_zero_or_more(cur_state, group_begin, group_end);

				//Skip next character
				cur_char++;
			}else if(*next_char == '+'){
				// Next character is a +
				// This matches one or more instances of the current character
				cur_state = regex_link_one_or_more(cur_state, group_begin, group_end);

				//Skip next character
				cur_char++;
			}
		} else if (*cur_char == '|'){
			FA_Graph graph;
			// Next character is a |
			// This means we have to implement an or

			// Parse rest of string
			FA_Graph group_graph = regex_generate_NFA_from_regex(cur_char + 1);
			FA_State *group_2_begin = group_graph.begin;
			FA_State *group_2_end = group_graph.end;

			// create new start of NFA
			FA_State *nfa_begin = regex_create_empty_FA_state();

			cur_state = regex_link_or(nfa_begin,start_state,cur_state,group_2_begin,group_2_end);

			// Begin of NFA is the begin of the or
			start_state = nfa_begin;

			// And we're done, the recursion handled the rest of the string
			graph.begin = start_state;
			graph.end = cur_state;
			printf("returning after |\n");
			return graph;
		}

		cur_char++;
	}
	// We reached the end of the string, return
	graph.begin = start_state;
	graph.end = cur_state;
	printf("returning after \\0\n");

	return graph;
}

FA_State *regex_link_zero_or_more(FA_State *cur_state, FA_State *group_begin, FA_State *group_end){
	/*
	Create the following pattern
	              E
	         <--------
	        /         \
	   --->O---------->O----_>
	E /    B    a      E      \  E
	O------------------------>0
	            E             3
	*/

	// Allocate memory for new states
	FA_State *next_state = regex_create_empty_FA_state();

	// Link states according to above pattern
	regex_link_NFA_states(cur_state,group_begin,EPSILON);
	regex_link_NFA_states(cur_state,next_state,EPSILON);
	regex_link_NFA_states(group_end,group_begin,EPSILON);
	regex_link_NFA_states(group_end,next_state,EPSILON);

	return next_state;
}

FA_State *regex_link_one_or_more(FA_State *cur_state, FA_State *group_begin, FA_State *group_end){
	/*
	Create the following pattern
	                     E
	                <--------
	       E       /         \     E
	O----------->O---------->O-------->O
	c            B     a     E         3
	*/

	// Allocate memory for new states
	FA_State *next_state = regex_create_empty_FA_state();

	// Link states according to above pattern
	regex_link_NFA_states(cur_state,group_begin,EPSILON);
	regex_link_NFA_states(group_end,group_begin,EPSILON);
	regex_link_NFA_states(group_end,next_state,EPSILON);

	// Move state pointer to next state
	return next_state;
}

FA_State *regex_link_or(FA_State *cur_state, FA_State *group_1_begin, FA_State *group_1_end, FA_State *group_2_begin, FA_State *group_2_end){
	/*
	Create the following pattern
	        a
	   O-------->O
	E /           \  E
	 /             \
	O               O
	 \             /
	E \           / E
	   O-------->O
	       b
	*/

	// Allocate memory for new states
	FA_State *next_state = regex_create_empty_FA_state();

	// Link states according to above pattern
	regex_link_NFA_states(cur_state,group_1_begin,EPSILON);
	regex_link_NFA_states(cur_state,group_2_begin,EPSILON);
	regex_link_NFA_states(group_1_end,next_state,EPSILON);
	regex_link_NFA_states(group_2_end,next_state,EPSILON);

	// Move state pointer to next state
	return next_state;
}


char *regex_get_group(char *begin){
	size_t substr_length;
	char *substr, *copy_char, *substr_copy_char;
	// We found a group, create substring for group
	// First we need to find the matching parentheses
	int parentheses_level = 1;
	char *peek = begin;

	// Find matching parentheses by increasing the level for each opening,
	// and decrease for closing. If the level returns to zero, we found the matching parentheses.
	while (parentheses_level != 0){
		peek++;
		if (*peek == '(')
			parentheses_level++;
		else if(*peek == ')')
			parentheses_level--;
	}

	// Calculate the length of the substring and allocate memory
	substr_length = peek - begin;
	substr = (char *) malloc(substr_length * sizeof(char));

	// Copy substring to buffer
	copy_char = begin + 1;
	substr_copy_char = substr;
	while (copy_char != peek){
		*substr_copy_char = *copy_char;
		substr_copy_char++;
		copy_char++;
	}
	// Append string end
	*substr_copy_char = '\0';

	return substr;
}
