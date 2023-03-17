/* Copyright (c) 2008, 2009
 *      Juergen Weigert (jnweiger@immd4.informatik.uni-erlangen.de)
 *      Michael Schroeder (mlschroe@immd4.informatik.uni-erlangen.de)
 *      Micah Cowan (micah@cowan.name)
 *      Sadrul Habib Chowdhury (sadrul@users.sourceforge.net)
 * Copyright (c) 1993-2002, 2003, 2005, 2006, 2007
 *      Juergen Weigert (jnweiger@immd4.informatik.uni-erlangen.de)
 *      Michael Schroeder (mlschroe@immd4.informatik.uni-erlangen.de)
 * Copyright (c) 1987 Oliver Laumann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, see
 * https://www.gnu.org/licenses/, or contact Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 *
 ****************************************************************
 */

#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "include/config.h"
#include "include/misc.h"
#include "include/screen.h"

char* 
filename(char* s) {
    char* p = s;

    if (p) {
        while (*p) {
            if (*p++ == '/')
                s = p;
        }
    }

    return s;
}

char*
savestr(const char* s) {
    char* cp = strdup(s);

    if (cp == NULL)
        Panic(0, "%s", strnomem);

    return cp;
}

char*
savestrn(const char* s, size_t size) {
    char* cp = strndup(s, size + 1);

    if (cp == NULL)
        Panic(0, "%s", strnomem);

    return cp;
}

char* 
stripdev(char* name) {
    if (name == NULL)
        return NULL;

    if (strncmp(name, "/dev/", 5) == 0)
        return name + 5;

    return name;
}

void 
close_allfiles(int except) {
    struct pollfd pfd[1024];
    int max_fd, i, fd, ret, z;

    i = 3; // skip stdin, stdout and stderr
    max_fd = getdtablesize();

    while (i < max_fd) {
        memset(pfd, 0, sizeof(pfd));
        z = 0;

        for (fd = i; fd < max_fd && fd < i + 1024; fd++)
            pfd[z++].fd = fd;

        ret = poll(pfd, fd - i, 0);

        if (ret < 0)
            Panic(errno, "poll");

        z = 0;

        for (fd = i; fd < max_fd && fd < i + 1024; fd++) {
            if (!(pfd[z++].revents & POLLNVAL) && fd != except)
                close(fd);
        }

        i = fd;
    }
}

void 
centerline(char* str, int y) {
    int x, len;

    len = strlen(str);

    if (len > flayer->l_width - 1)
        len = flayer->l_width - 1;

    x = (flayer->l_width - 1 - len) / 2;

    LPutStr(flayer, str, len, &mchar_blank, x, y);
}

void 
leftline(char* s, int y, struct mchar* render) {
    int l, len;
    struct mchar mchar_dol;

    mchar_dol = mchar_blank;
    mchar_dol.image = '$';

    l = len = strlen(s);

    if (len > flayer->l_width - 1)
        len = flayer->l_width - 1;

    LPutStr(flayer, s, len, render ? render : &mchar_blank, 0, y);

    if (len != l)
        LPutChar(flayer, &mchar_dol, len, y);
}

// -------
// signal handling
// -------
void 
(*xsignal(int sig, void (*cb)(int))) (int) {
    struct sigaction osa, sa;

    sa.sa_handler = cb;

    (void) sigemptyset(&sa.sa_mask);

#ifdef SA_RESTART
    sa.sa_flags = (sig == SIGCHLD ? SA_RESTART : 0);
#else
    sa.sa_flags = 0;
#endif

    if (sigaction(sig, &sa, &osa))
        return (void (*)(int)) -1;

    return osa.sa_handler;
}

// -------
// uid/gid handling
// -------
void 
xseteuid(int euid) {
    if (seteuid(euid) == 0)
        return;

    if (seteuid(0) || seteuid(euid))
        Panic(errno, "seteuid");
}

void 
xsetegid(int egid) {
    if (setegid(egid))
        Panic(errno, "setegid");
}

// -------
// security: switch to real uid
// -------
static int user_stat;

int 
add_xchar(char* buf, int ch) {
    char* p = buf;

    if (ch < ' ' || ch == /* DEL */ 0x7f) {
        *p++ = '^';
        // if (ch == DEL) ?
        // if (ch < ' ') rollback on ascii table
        *p++ = ch ^ 0x40;
    } else if (ch >= 0x80) {
        *p++ = '\\';
        *p++ = (ch >> 6 & 7) + '0';
        *p++ = (ch >> 3 & 7) + '0';
        *p++ = (ch >> 0 & 7) + '0';
    } else
        *p++ = ch;

    return p - buf;
}

int 
add_xchars(char* buf, int len, char* s) {
    char *p;

    if (s == NULL) {
        *buf = 0;

        return 0;
    }

    len -= 4; // longest sequence produced by scr_add_xchar()

    for (p = buf; p < buf + len && *s; s++) {
        if (*s == ' ')
            *p++ = *s;
        else
            p += add_xchar(p, *s);
    }

    *p = 0;

    return p - buf;
}

int 
user_context(void) {
    xseteuid(real_uid);
    xsetegid(real_gid);

    return 1;
}

int 
user_status(void) {
    return user_stat;
}

void 
user_return(int val) {
    xseteuid(eff_uid);
    xsetegid(eff_gid);
    user_stat = val;
}
