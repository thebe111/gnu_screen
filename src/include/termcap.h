#ifndef SCREEN_TERMCAP_H
#define SCREEN_TERMCAP_H

#include <stdbool.h>

char* MakeTermcap (bool);
char* gettermcapstring (char*);
int CreateTransTable (char*);
int InitTermcap (int, int);
int remap (int, int);
void CheckEscape (void);
void FreeTransTable (void);

/* == GLOBAL VARIABLES == */

extern char screenterm[];
extern char Term[];
extern char Termcap[];
extern char* extra_incap;
extern char* extra_outcap;

#endif // SCREEN_TERMCAP_H
