#ifndef SCREEN_SOCKET_H
#define SCREEN_SOCKET_H

#include "window.h"

int FindSocket (int*, int*, int*, char*);
int MakeClientSocket (int);
int MakeServerSocket (void);
int RecoverSocket (void);
int SendAttachMsg (int, Message*, int);
int SendErrorMsg (char*, char*);
int chsock (void);
void ReceiveMsg (void);
void ReceiveRaw (int);
void SendCreateMsg (char*, struct NewWindow*);

#endif // SCREEN_SOCKET_H
