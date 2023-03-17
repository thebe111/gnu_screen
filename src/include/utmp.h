#ifndef SCREEN_UTMP_H
#define SCREEN_UTMP_H

#include "config.h"
#include "window.h"

void  SlotToggle (bool);

#ifdef ENABLE_UTMP
int RemoveUtmp (Window* win);
int SetUtmp (Window* win);
void InitUtmp (void);
void RemoveLoginSlot (void);
void RestoreLoginSlot (void);
#endif // ENABLE_UTMP

#ifdef CAREFULUTMP
void CarefulUtmp (void);
#else
#define CarefulUtmp() // NOTE: nothing
#endif // CAREFULUTMP

#endif // SCREEN_UTMP_H
