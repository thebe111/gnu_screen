#ifndef SCREEN_ATTACHER_H
#define SCREEN_ATTACHER_H

int Attach (int);
__attribute__((__noreturn__)) void Attacher (void);
__attribute__((__noreturn__)) void AttacherFinit (int);
void SendCmdMessage (char*, char*, char**, int);

#endif // SCREEN_ATTACHER_H
