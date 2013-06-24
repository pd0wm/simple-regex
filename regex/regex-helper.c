#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "regex.h"
#include "dfa.h"
#include "nfa.h"


void regex_state_list_append(FA_StateListItem **list1, FA_StateListItem *list2){
	// Check if list exists
	if (*list1 == NULL){
		*list1 = list2;
	}else{
		// Find last
		FA_StateListItem *item = *list1;
		while(item->next != NULL)
			item = item->next;
		// Add item to list
		item->next = list2;
	}
}

FA_StateListItem *regex_state_list_pop(FA_StateListItem **list){
	// Find last
	FA_StateListItem *item = *list;
	FA_StateListItem *prev = NULL;

	while(item->next != NULL){
		prev = item;
		item = item->next;
	}

	// Remove last item from list
	if (prev)
		prev->next = NULL;
	// If we pop last item from list, set it to NULL
	else
		*list = NULL;

	return item;
}
void regex_state_list_push(FA_StateListItem **list, FA_State *state){
	FA_StateListItem *new_state = (FA_StateListItem *) malloc( sizeof(FA_StateListItem));
	new_state->state = state;
	new_state->next = NULL;

	// Check if list exists
	if (*list == NULL){
		*list = new_state;
	}else{
		// Find last
		FA_StateListItem *item = *list;
		while(item->next != NULL)
			item = item->next;
		// Add item to list
		item->next = new_state;
	}
}

FA_State *regex_create_empty_FA_state(void){
	FA_State *next_state = (FA_State*) malloc( sizeof(FA_State) );
	next_state->transitions = NULL;
	next_state->NFA_states = NULL;
	next_state->original_NFA_state = NULL;
	next_state->end = 0;

	printf("Created FA state: %d\n", (int)next_state);

	return next_state;
}


/*
Link two states by creating a transition, and adding this transistion to the
outbound transistion list of the outbound state.
*/
void regex_link_NFA_states(FA_State *A, FA_State *B, char condition){
	FA_TransitionListItem *list_item;
	// Create transition and populate member variables
	FA_Transition *transition = (FA_Transition*) malloc( sizeof(FA_Transition));
	transition->condition = condition;
	transition->from = A;
	transition->to = B;

	if (condition == '\0')
		printf("Linking %d -> %d with condition EPSILON: %d\n", (int) A,(int) B,(int) transition);
	else
		printf("Linking %d -> %d with condition %c: %d\n", (int) A,(int) B, condition,(int) transition);

	// Create list item for A
	list_item = (FA_TransitionListItem*) malloc( sizeof(FA_TransitionListItem));
	list_item->transition = transition;
	list_item->next = NULL;
	// Add to outbound transition list
	regex_add_NFA_transition_to_list(A,list_item);
}

/*
Add a transition to the linked list of transitions
*/
void regex_add_NFA_transition_to_list(FA_State *state, FA_TransitionListItem * transition){
	// Check if list exists
	if (state->transitions == NULL){
		state->transitions = transition;
	}else{
		// Find last
		FA_TransitionListItem *item = state->transitions;
		while(item->next != NULL)
			item = item->next;
		// Add item to list
		item->next = transition;
	}
}




