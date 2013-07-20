#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "regex-helper.h"
#include "regex-dfa.h"

FA_Graph regex_generate_DFA_from_NFA(FA_Graph NFA){
	FA_State *NFA_begin = NFA.begin;
	FA_Graph graph;
	FA_StateListItem *work_list = (FA_StateListItem *) malloc( sizeof(FA_StateListItem));
	FA_StateListItem *DFA_states_list = NULL;
	FA_State *start_state = regex_create_empty_FA_state();

	start_state->NFA_states = regex_epsilon_closure(NFA_begin);

	start_state->original_NFA_state = NFA_begin;

	regex_state_list_push(&DFA_states_list,start_state);

	// Add first state to work list
	work_list->state = start_state;
	work_list->next = NULL;

	while (work_list != NULL){
		FA_StateListItem *state_item;
		FA_State *state;
		FA_StateListItem *nfa_state_item;

		state_item = regex_state_list_pop(&work_list);

		state = state_item->state;

		// For each corresponding NFA state we have to check for transition
		nfa_state_item = state->NFA_states;

		while(nfa_state_item){
			FA_TransitionListItem *transition_item;
			FA_State *NFA_state =  nfa_state_item->state;
			printf("Working on: %d\n", (int)NFA_state);

			// Now for each non epsilon transition, we must create a dfa state
			transition_item = NFA_state->transitions;
			while (transition_item){
				FA_Transition *transition = transition_item->transition;


				if (transition->condition != EPSILON){
					FA_State *new_state;
					printf("Transition %d\n", (int)transition);

					new_state = regex_find_DFA_state_by_original_FA_state(DFA_states_list,transition->to);
					// Check if state is already in list
					if (new_state){
						printf("In list\n");
					}else{
						printf("Not in list: %d\n", (int)transition->to);
						new_state = regex_create_empty_FA_state();
						new_state->original_NFA_state = transition->to;
						regex_state_list_push(&DFA_states_list, new_state);
						regex_state_list_push(&work_list, new_state);

						new_state->NFA_states = regex_epsilon_closure(transition->to);
					}

					if (regex_FA_state_is_in_list(new_state->NFA_states, NFA.end)){
						new_state->end = 1;
					}

					// Link current state to new state
					regex_link_NFA_states(state,new_state,transition->condition);
				}
				transition_item = transition_item->next;

			}
			nfa_state_item = nfa_state_item->next;
		}
	}

	graph.begin = start_state;
	graph.end = start_state;
	return graph;
}

FA_StateListItem *regex_epsilon_closure(FA_State *state){
	FA_TransitionListItem *transition_list_pointer;
	// Create empty list and add current state
	FA_StateListItem *closure_list = (FA_StateListItem *) malloc( sizeof(FA_StateListItem));
	closure_list->state = state;
	closure_list->next = NULL;

	// Call function recursively on all epsilon transitions
	transition_list_pointer = state->transitions;
	while (transition_list_pointer != NULL){
		if (transition_list_pointer->transition->condition == EPSILON){
			FA_State *to = transition_list_pointer->transition->to;
			if (!regex_FA_state_is_in_list(closure_list,to)){
				FA_StateListItem *recursive_closure_list = regex_epsilon_closure(to);
				regex_state_list_append(&closure_list,recursive_closure_list);
			}
		}
		transition_list_pointer = transition_list_pointer->next;
	}

	return closure_list;
}


int regex_FA_state_is_in_list(FA_StateListItem *list, FA_State *state){
	FA_StateListItem *item = list;
	while(item){
		if (item->state == state)
			return 1;
		item = item->next;
	}
	return 0;
}

FA_State *regex_find_DFA_state_by_original_FA_state(FA_StateListItem *list, FA_State *state){
	// Print state list
	FA_StateListItem *item = list;
	while(item){
		if (item->state->original_NFA_state == state){
			return item->state;
		}
		item = item->next;
	}
	return NULL;
}
