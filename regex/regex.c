#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "regex.h"

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

		printf("Work list size: %d\n", regex_state_list_size(work_list));
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

int regex_state_list_size(FA_StateListItem *list){
	int i = 0;
	FA_StateListItem *item = list;
	while(item){
		i++;
		item = item->next;
	}
	return i;
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
