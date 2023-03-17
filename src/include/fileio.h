#ifndef SCREEN_FILEIO_H
#define SCREEN_FILEIO_H

#include "window.h"

FILE* secfopen (char*, char*);
char* ReadFile (char*, int*);
int StartRc (char*, int);
int printpipe (Window* win, char*);
int readpipe (char**);
int secopen (char*, int, int);
void FinishRc (char*);
void KillBuffers (void);
void RcLine (char*, int);
void WriteFile (struct acluser*, char*, int);
void do_source (char*);

/* == GLOBAL VARIABLES == */

extern char *rc_name;

#endif
