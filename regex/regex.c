#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "regex.h"

/*
	This function generates a NFA graph for the given regex
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
			// Normal charater check next character
			char *next_char = cur_char + 1;
			if (isalnum( (int)(*next_char) ) || *next_char == '(' || *next_char == ')' || *next_char == '\0'){
				// Next character is a normal character or group start or group end
				// So just create a next state
				//    a
				// O------>O
				NFA_State *next_state = (NFA_State*) malloc( sizeof(NFA_State) );
				regex_link_NFA_states(cur_state,next_state,*cur_char);

				// Move state pointer to next state
				cur_state = next_state;

				// If it is a group end, we should return
				if (*next_char == ')')
					return start_state;
			}else{
				if(*next_char == '*'){
					// Next character is a *
					// This matches zero or more instances of the current character
					// Create the following pattern
					//             E
					//         <--------
					//        /         \
					//   --->O---------->O----_>
					//E /    1    a      2      \  E
					// O------------------------>0
					//             E             3

					// Allocate memory for new states
					NFA_State *next_state_1 = (NFA_State*) malloc( sizeof(NFA_State) );
					NFA_State *next_state_2 = (NFA_State*) malloc( sizeof(NFA_State) );
					NFA_State *next_state_3 = (NFA_State*) malloc( sizeof(NFA_State) );

					// Link states according to above pattern
					regex_link_NFA_states(cur_state,next_state_1,EPSILON);
					regex_link_NFA_states(cur_state,next_state_3,EPSILON);
					regex_link_NFA_states(next_state_1,next_state_2,*cur_char);
					regex_link_NFA_states(next_state_2,next_state_1,EPSILON);
					regex_link_NFA_states(next_state_2,next_state_3,EPSILON);

					// Move state pointer to next state
					cur_state = next_state_3;

					//Skip next character
					cur_char++;
				}else if(*next_char == '+'){
					// Next character is a +
					// This matches one or more instances of the current character
					// Create the following pattern
					//                    E
					//                <--------
					//       E       /         \     E
					// O----------->O---------->O-------->O
					// c            1     a     2         3

					// Allocate memory for new states
					NFA_State *next_state_1 = (NFA_State*) malloc( sizeof(NFA_State) );
					NFA_State *next_state_2 = (NFA_State*) malloc( sizeof(NFA_State) );
					NFA_State *next_state_3 = (NFA_State*) malloc( sizeof(NFA_State) );

					// Link states according to above pattern
					regex_link_NFA_states(cur_state,next_state_1,EPSILON);
					regex_link_NFA_states(next_state_1,next_state_2,*cur_char);
					regex_link_NFA_states(next_state_2,next_state_1,EPSILON);
					regex_link_NFA_states(next_state_2,next_state_3,EPSILON);

					// Move state pointer to next state
					cur_state = next_state_3;

					//Skip next character
					cur_char++;
				}
			}
		} else if (*cur_char == '('){
			// Get substring for group and call this function recursively on the substring
			char *substr = regex_get_group(cur_char);

			// Move cur_char to end of group
			cur_char += strlen(substr) + 1;


			// Get begin and end of group
			NFA_State *group_begin = regex_generate_NFA_from_regex(substr);
			NFA_State *group_end = regex_get_end_of_NFA(group_begin);

			// Free memory for substr
			free(substr);
			substr = NULL;

			char *next_char = cur_char + 1;

			if(*next_char == '*'){
				// Next character is a *
				// This matches zero or more instances of the current character
				// Create the following pattern
				//             E
				//         <--------
				//        /         \
				//   --->O---------->O----_>
				//E /    B    a      E      \  E
				// O------------------------>0
				//             E             3

				// Allocate memory for new states
				NFA_State *next_state_3 = (NFA_State*) malloc( sizeof(NFA_State) );

				// Link states according to above pattern
				regex_link_NFA_states(cur_state,group_begin,EPSILON);
				regex_link_NFA_states(cur_state,next_state_3,EPSILON);
				regex_link_NFA_states(group_end,group_begin,EPSILON);
				regex_link_NFA_states(group_end,next_state_3,EPSILON);

				// Move state pointer to next state
				cur_state = next_state_3;

				//Skip next character
				cur_char++;
			}else if(*next_char == '+'){
					// Next character is a +
					// This matches one or more instances of the current character
					// Create the following pattern
					//                    E
					//                <--------
					//       E       /         \     E
					// O----------->O---------->O-------->O
					// c            B     a     E         3

					// Allocate memory for new states
					NFA_State *next_state_3 = (NFA_State*) malloc( sizeof(NFA_State) );

					// Link states according to above pattern
					regex_link_NFA_states(cur_state,group_begin,EPSILON);
					regex_link_NFA_states(group_end,group_begin,EPSILON);
					regex_link_NFA_states(group_end,next_state_3,EPSILON);

					// Move state pointer to next state
					cur_state = next_state_3;

					//Skip next character
					cur_char++;
				}

		}
		cur_char++;
	}

	return start_state;
}

/*
	Link two states by creating a transition, and adding this transistion to the
	outbound transistion list of the outbound state.
*/
void regex_link_NFA_states(NFA_State *A, NFA_State *B, char condition){
	if (condition == '\0')
		printf("Linking with condition EPSILON\n");
	else
		printf("Linking with condition %c\n", condition);
	// Create transition and populate member variables
	NFA_Transition *transition = (NFA_Transition*) malloc( sizeof(NFA_Transition));
	transition->condition = condition;
	transition->from = A;
	transition->to = B;

	// Create list item for A
	NFA_TransitionListItem * list_item = (NFA_TransitionListItem*) malloc( sizeof(NFA_TransitionListItem));
	list_item->transition = transition;
	// Add to outbound transition list
	regex_add_NFA_transition_to_list(A,list_item);
}

/*
	Add a transition to the linked list of transitions
*/
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

NFA_State *regex_get_end_of_NFA(NFA_State *begin){
	NFA_State *cur = begin;

	while (cur->transitions != NULL){
		printf("Following: %c\n", cur->transitions->transition->condition);
		cur = cur->transitions->transition->to;
	}
	return cur;
}

char *regex_get_group(char *begin){
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
	size_t substr_length = peek - begin;
	char *substr = malloc(substr_length * sizeof(char));

	// Copy substring to buffer
	char *copy_char = begin + 1;
	char *substr_copy_char = substr;
	while (copy_char != peek){
		*substr_copy_char = *copy_char;
		substr_copy_char++;
		copy_char++;
	}
	// Append string end
	*substr_copy_char = '\0';

	return substr;
}
