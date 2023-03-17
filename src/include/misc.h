#ifndef SCREEN_MISC_H
#define SCREEN_MISC_H

#include "config.h"
#include "image.h"
#include <sys/types.h>

char* filename(char* s);
char* savestr(const char* s);
char* savestrn(const char* s, size_t size);
char* stripdev(char* name);
void close_allfiles (int except);
void centerline(char* s, int y);
void leftline (char* s, int y, struct mchar* render); // @@@ mchar_t

// -------
// signal handling
// -------
void (*xsignal (int sig, void (*cb)(int))) (int);

// -------
// uid/gid handling
// -------
#if defined(HAVE_SETEUID) || defined(HAVE_SETREUID)
void xseteuid (int);
void xsetegid (int);
#endif

int add_xchar(char* buf, int ch);
int add_xchars(char* buf, int len, char* s);
int user_context(void);
int user_status(void);
void user_return(int val);

// FIXME: is not in use ???
#ifndef HAVE_STRERROR
char* strerror (int);
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#endif // SCREEN_MISC_H
