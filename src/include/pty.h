#ifndef SCREEN_PTY_H
#define SCREEN_PTY_H

int open_pty(char** ttyn);
int close_pty(int fd);

/* == GLOBAL VARIABLES == */

extern int pty_preopen;

#endif // SCREEN_PTY_H
