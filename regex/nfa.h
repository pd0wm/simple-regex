#ifndef NFA_H
#define NFA_H

#include "regex-helper.h"

FA_Graph regex_generate_NFA_from_regex(char * regex);
char *regex_get_group(char *begin);
FA_State *regex_link_zero_or_more(FA_State *cur_state, FA_State *group_begin, FA_State *group_end);
FA_State *regex_link_one_or_more(FA_State *cur_state, FA_State *group_begin, FA_State *group_end);
FA_State *regex_link_or(FA_State *cur_state, FA_State *group_1_begin, FA_State *group_1_end, FA_State *group_2_begin, FA_State *group_2_end);
#endif
