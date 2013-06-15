#ifndef REGEX_H
#define REGEX_H

typedef struct NFA_Transition NFA_Transition;
typedef struct NFA_TransitionListItem NFA_TransitionListItem;
typedef struct NFA_State NFA_State;

struct NFA_Transition{
	char condition;
	NFA_State *from;
	NFA_State *to;
};

struct NFA_TransitionListItem{
	NFA_Transition transition;
	NFA_TransitionListItem *next;
};


struct NFA_State{
	NFA_TransitionListItem * transitions;
};

NFA_State *regex_generate_NFA_from_regex(char * regex);
void regex_link_NFA_states(NFA_State *A, NFA_State *B, char condition);
void regex_add_NFA_transition_to_list(NFA_State *state, NFA_TransitionListItem * transition);
#endif
