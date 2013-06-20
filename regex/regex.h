#ifndef REGEX_H
#define REGEX_H

#define EPSILON '\0'

typedef struct FA_Transition FA_Transition;
typedef struct FA_TransitionListItem FA_TransitionListItem;
typedef struct FA_StateListItem FA_StateListItem;
typedef struct FA_State FA_State;
typedef struct FA_Graph FA_Graph;

struct FA_Transition{
	char condition;
	FA_State *from;
	FA_State *to;
};

struct FA_TransitionListItem{
	FA_Transition *transition;
	FA_TransitionListItem *next;
};

struct FA_State{
	FA_State *original_NFA_state;
	FA_StateListItem *NFA_states;
	FA_TransitionListItem *transitions;
	int end;
};

struct FA_StateListItem{
	FA_State *state;
	FA_StateListItem *next;
};

struct FA_Graph{
	FA_State *begin;
	FA_State *end;
};



FA_Graph regex_generate_DFA_from_NFA(FA_Graph NFA);
FA_StateListItem *regex_epsilon_closure(FA_State *state);
int regex_FA_state_is_in_list(FA_StateListItem *list, FA_State *state);
void regex_state_list_append(FA_StateListItem **list1, FA_StateListItem *list2);
FA_StateListItem *regex_state_list_pop(FA_StateListItem **list);
void regex_state_list_push(FA_StateListItem **list, FA_State * state);
FA_State *regex_create_empty_FA_state(void);
FA_Graph regex_generate_NFA_from_regex(char * regex);
void regex_link_NFA_states(FA_State *A, FA_State *B, char condition);
void regex_add_NFA_transition_to_list(FA_State *state, FA_TransitionListItem * transition);
char *regex_get_group(char *begin);
FA_State *regex_link_zero_or_more(FA_State *cur_state, FA_State *group_begin, FA_State *group_end);
FA_State *regex_link_one_or_more(FA_State *cur_state, FA_State *group_begin, FA_State *group_end);
FA_State *regex_link_or(FA_State *cur_state, FA_State *group_1_begin, FA_State *group_1_end, FA_State *group_2_begin, FA_State *group_2_end);
int regex_state_list_size(FA_StateListItem *list);
FA_State *regex_find_DFA_state_by_original_FA_state(FA_StateListItem *list, FA_State *state);

#endif
