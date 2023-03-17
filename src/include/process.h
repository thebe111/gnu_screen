#ifndef SCREEN_PROCESS_H
#define SCREEN_PROCESS_H

#include "winmsgbuf.h"

Window* FindNiceWindow (Window* win, char*);
char* AddOtherUsers (char*, int, Window* win);
char* AddWindowFlags (char*, int, Window* win);
char* AddWindows (win_msg_buf_context_t* ctx, int, int, int);
int CompileKeys (char*, int, unsigned char*);
int FindCommnr (const char*);
int IsNumColon (char*, char*, int);
int Parse (char*, int, char**, int*);
int StuffKey (int);
int WindowByNoN (char*);
uint64_t ParseAttrColor (char*, int);
void Activate (int);
void ApplyAttrColor (uint64_t, struct mchar*);
void DoAction  (struct action* action);
void DoCommand (char**, int*);
void DoProcess (Window* win, char**, size_t*, struct paster* paster);
void DoScreen (char*, char**);
void InitKeytab (void);
void KillWindow (Window* win);
void ProcessInput (char*, size_t);
void ProcessInput2 (char*, size_t);
void RefreshXtermOSC (void);
void SetEscape (struct acluser* acluser, int, int);
void SetForeWindow (Window* win);
void ShowWindows (int);
void SwitchWindow (Window* win);

/* == GLOBAL VARIABLES == */

extern bool hardcopy_append;
extern char *noargs[];
extern char *zmodem_recvcmd;
extern char *zmodem_sendcmd;
extern char NullStr[];
extern int TtyMode;
extern int idletimo;
extern int kmap_extn;
extern int zmodem_mode;
extern struct action dmtab[];
extern struct action idleaction;
extern struct action ktab[];
extern struct action mmtab[];
extern struct action umtab[];
extern struct kmap_ext *kmap_exts;

#endif // SCREEN_PROCESS_H
