#ifndef SCREEN_HELP_H
#define SCREEN_HELP_H

#include "comm.h"
#include "canvas.h"

int InWList (void);
void WListLinkChanged (void);
void WListUpdatecv (canvas_t* canvas, Window* win);
void ZmodemPage (void);
void display_bindkey (char*, struct action*);
void display_help (char*, struct action*);

/* == GLOBAL VARIABLES == */

extern char version[];

#endif // SCREEN_HELP_H
