#ifndef REGEX_H
#define REGEX_H

#define EPSILON '\0'

typedef struct NFA_Transition NFA_Transition;
typedef struct NFA_TransitionListItem NFA_TransitionListItem;
typedef struct NFA_State NFA_State;
typedef struct NFA_Graph NFA_Graph;

struct NFA_Transition{
	char condition;
	NFA_State *from;
	NFA_State *to;
};

struct NFA_TransitionListItem{
	NFA_Transition *transition;
	NFA_TransitionListItem *next;
};


struct NFA_State{
	NFA_TransitionListItem *transitions;
};

struct NFA_Graph{
	NFA_State *begin;
	NFA_State *end;
};

NFA_Graph regex_generate_NFA_from_regex(char * regex);
void regex_link_NFA_states(NFA_State *A, NFA_State *B, char condition);
void regex_add_NFA_transition_to_list(NFA_State *state, NFA_TransitionListItem * transition);
char *regex_get_group(char *begin);
NFA_State *regex_link_zero_or_more(NFA_State *cur_state, NFA_State *group_begin, NFA_State *group_end);
NFA_State *regex_link_one_or_more(NFA_State *cur_state, NFA_State *group_begin, NFA_State *group_end);
NFA_State *regex_link_or(NFA_State *cur_state, NFA_State *group_1_begin, NFA_State *group_1_end, NFA_State *group_2_begin, NFA_State *group_2_end);
#endif
