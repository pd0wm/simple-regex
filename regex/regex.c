#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "regex.h"

/*
	This function generates a NFA state diagram for the given regex
*/
NFA_State *regex_generate_NFA_from_regex(char *regex) {
	// Print regex
	printf("Regex: %s\n", regex);

	// Alocate memory for first state
	NFA_State *start_state = (NFA_State*) malloc( sizeof(NFA_State) );
	start_state->transitions = NULL;

	NFA_State *cur_state = start_state;

	// Process each character of the regex
	char *cur_char = regex;
	while (*cur_char != '\0') {
		printf("%c\n", *cur_char);

		// Check if cur_char is not a special character
		if ( isalnum( (int)(*cur_char) ) ) {
			// Normal charater, create new state and link
			NFA_State *next_state = (NFA_State*) malloc( sizeof(NFA_State) );
			regex_link_NFA_states(cur_state,next_state,*cur_char);
		}

		cur_char++;
	}

	return start_state;
}


void regex_link_NFA_states(NFA_State *A, NFA_State *B, char condition){
	// Create transition
	NFA_Transition *transition = (NFA_Transition*) malloc( sizeof(NFA_Transition));
	transition->condition = condition;
	transition->from = A;
	transition->to = B;

	// Create list item for A
	NFA_TransitionListItem * list_item = (NFA_TransitionListItem*) malloc( sizeof(NFA_TransitionListItem));
	// Add to outbound transition list
	regex_add_NFA_transition_to_list(A,list_item);
}

void regex_add_NFA_transition_to_list(NFA_State *state, NFA_TransitionListItem * transition){
	// Check if list exists
	if (state->transitions == NULL){
		state->transitions = transition;
	}else{
		// Find last
		NFA_TransitionListItem *item = state->transitions;
		while(item->next != NULL)
			item = item->next;
		// Add item to list
		item->next = transition;
	}
}
