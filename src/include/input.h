#ifndef SCREEN_INPUT_H
#define SCREEN_INPUT_H

#include <stdlib.h>

int InInput (void);
void Input (char*, size_t, int, void (*)(char*, size_t, void*), char*, int);
void inp_setprompt (char*, char*);

#endif // SCREEN_INPUT_H
