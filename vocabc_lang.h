#ifndef VOCABC_LANG_H
#define VOCABC_LANG_H
#include <stdlib.h>

#define CORRECT 0
#define WRONG 1
#define CORR_WAS 2
#define WORD 3
#define OF 4
#define KNOWN 5
#define PROGRESS 6
#define ANALYSIS1 7
#define ANALYSIS2 8

#define VERS 0
#define DEVELOP 1
#define STATUS 2
#define VOCFILE 3
#define RANDOM 4
#define CASE 5
#define DIRECTION 6
#define BOTH 7
#define COMMENT 8
#define PAIRS 9
#define QUERY_NUM 10
#define AV_PER 11
#define TOP_PER 12

#define EXIT 0

extern char *query_strings[3][10];

extern char *status_strings[3][13];

extern char *program_strings[3][1];

#endif
