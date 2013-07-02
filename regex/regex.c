#include "regex-dfa.h"
#include "regex-nfa.h"

#include <stdio.h>
#include <stdlib.h>

FA_State *regex_advance_state(FA_State * state, char c){
	FA_TransitionListItem *transition;
	// Loop throug all conditions

	transition = state->transitions;
	while (transition != NULL){

		if (transition->transition->condition == c)
			return transition->transition->to;
		transition = transition->next;
	}

	return NULL;
}



void regex_match(char * regex, char * string){
	//Compile regex
	FA_Graph NFA, DFA;
	FA_StateListItem *cur_states = NULL;
	FA_StateListItem *state_iterator;
	char *cur;

	printf("Create NFA -----------------\n");
	NFA = regex_generate_NFA_from_regex(regex);
	printf("Create DFA -----------------\n");
	DFA = regex_generate_DFA_from_NFA(NFA);

	printf("End node: %d\n", (int)DFA.end);

	cur = string;
	while (*cur != '\0'){
		FA_StateListItem *new_states = NULL;

		regex_state_list_push_match(&cur_states,DFA.begin,cur);
		while (state_iterator = regex_state_list_pop(&cur_states)){
			FA_State *next = regex_advance_state(state_iterator->state, *cur);


			if (next != NULL){
				regex_state_list_push_match(&new_states, next, state_iterator->match_start);
			}else{
				if (state_iterator->state->end){
					char *c = state_iterator->match_start;
					printf("Match: ");
					while (c != cur){
						printf("%c", *c);
						c++;
					}
					printf("\n");
				}
			}


			if (cur_states == NULL)
				break;
		}

		cur_states = new_states;
		cur++;
	}

	// And check states that are still in the matching process
	while (state_iterator = regex_state_list_pop(&cur_states)){
		if (state_iterator->state->end){
			char *c = state_iterator->match_start;
			printf("Match: ");
			while (c != cur){
				printf("%c", *c);
				c++;
			}
			printf("\n");
		}

		if (cur_states == NULL)
			break;
	}

}