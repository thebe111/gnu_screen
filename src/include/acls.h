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

#ifndef SCREEN_ACLS_H
#define SCREEN_ACLS_H

#include "os.h"

// three known bits:
#define ACL_EXEC 0      
#define ACL_WRITE 1
#define ACL_READ 2

#define ACL_BITS_PER_CMD 1 // FIXME: for comm.h ???
#define ACL_BITS_PER_WIN 3 // FIXME: for window.h ???

#define USER_CHUNK 1

#define ACLBYTE(data, w) ((data)[(w) >> 3])
#define ACLBIT(w) (0x80 >> ((w) & 7))

typedef struct window_s window_t;
typedef unsigned char* AclBits;

// how a user joins a group.
// here is the node to construct one list per user.
typedef struct aclusergroup {
  struct acluser *u; // the user who borrows us his rights
  struct aclusergroup *next;
} aclusergroup_t;

// FIXME: move to user.h ???

typedef struct plop {
    char *buf;
    int enc;
    size_t len;
} plop_t;

// A User has a list of groups, and points to other users.
// users is the User entry of the session owner (creator)
// and anchors all other users. Add/Delete users there.
typedef struct acluser {
  AclBits u_umask_w_bits[ACL_BITS_PER_WIN];   // his window create umask
  char u_name[MAXLOGINLEN + 1];               // login name how he showed up
  int u_Esc, u_MetaEsc;                      // the users screen escape character
  int u_detachotherwin;                      // window that was "other" when he detached
  int u_detachwin;                           // the window where he last detached
  int u_id;                                   // a uniq index in the bitfields
  struct acluser *u_next;                     // continue the main user list
  struct aclusergroup *u_group;               // linked list of pointers to other users
  struct plop u_plop;                        
} user_t;

// forward declaration
struct comm;

char *DoSu (struct acluser**, char*, char*, char*);
int AclCheckPermCmd(struct acluser*, int, struct comm*);
int AclCheckPermWin(struct acluser*, int, window_t* win);
int AclLinkUser (char*, char*);
int AclSetPerm (struct acluser*, struct acluser*, char *, char *);
int AclUmask (struct acluser*, char*, char **);
int NewWindowAcl(Window* win, struct acluser*);
int UserAdd (char*, struct acluser**);
int UserDel (char*, struct acluser**);
int UserFreeCopyBuffer (struct acluser*);
int UsersAcl (struct acluser*, int, char**);
struct acluser** FindUserPtr (char*);
void AclWinSwap (int, int);
void FreeWindowAcl(Window* win);

/* == GLOBAL VARIABLES == */

extern int DefaultEsc, DefaultMetaEsc;
extern int maxusercount;
extern struct acluser *users, *EffectiveAclUser;

#endif // SCREEN_ACLS_H
