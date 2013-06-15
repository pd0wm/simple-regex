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
	int id;
	NFA_TransitionListItem transition;
};

NFA_State *regex_generate_NFA_from_regex(char * regex);

#endif
