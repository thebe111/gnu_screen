#ifndef SCREEN_TTY_H
#define SCREEN_TTY_H

#include "screen.h"

int OpenTTY (char*, char*);
int CloseTTY (int);
void InitTTY (struct mode* mode, int);
void GetTTY (int, struct mode* mode);
void SetTTY (int, struct mode* mode);
void SetMode (struct mode* from, struct mode* to, int, int);
void SetFlow (bool);
void SendBreak (Window* win, int, int);
int TtyGrabConsole (int, bool, char*);
char* TtyGetModemStatus (int, char*);
void brktty (int);
int fgtty(int fd);
int CheckTtyname (char*);
char* GetPtsPathOrSymlink (int);

/* == GLOBAL VARIABLES == */

extern bool separate_sids;
extern int breaktype;

#endif // SCREEN_TTY_H
