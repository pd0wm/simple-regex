#ifndef REGEX_DFA_H
#define REGEX_DFA_H

#include "regex-helper.h"

FA_Graph regex_generate_DFA_from_NFA(FA_Graph NFA);
FA_StateListItem *regex_epsilon_closure(FA_State *state);
int regex_FA_state_is_in_list(FA_StateListItem *list, FA_State *state);
FA_State *regex_find_DFA_state_by_original_FA_state(FA_StateListItem *list, FA_State *state);



#endif
