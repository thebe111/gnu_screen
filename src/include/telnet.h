#ifndef SCREEN_TELNET_H
#define SCREEN_TELNET_H

#include "config.h"
#include "window.h"

#ifdef ENABLE_TELNET
int DoTelnet(char*, size_t*, int);
int TelIn(Window* win, char*, size_t, int);
int TelIsline(Window* win);
int TelOpenAndConnect(Window* win);
void TelBreak(Window* win);
void TelProcessLine(char**, size_t*);
void TelStatus(Window* win, char*, size_t);
void TelWindowSize(Window* win);
#endif

#endif // SCREEN_TELNET_H
