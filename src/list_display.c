/* Copyright (c) 2010
 *      Juergen Weigert (jnweiger@immd4.informatik.uni-erlangen.de)
 *      Sadrul Habib Chowdhury (sadrul@users.sourceforge.net)
 * Copyright (c) 2008, 2009
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

#include <stdbool.h>
#include <stdint.h>
#include "include/config.h"
#include "include/list_generic.h"
#include "include/misc.h"
#include "include/screen.h"
#include "include/acls.h"

static char listid[] = "display";

// layout of the displays page is as follows:
// 
// xterm 80x42      jnweiger@/dev/ttyp4    0(m11)    &rWx
// facit 80x24 nb   mlschroe@/dev/ttyhf   11(tcsh)    rwx
// xterm 80x42      jnhollma@/dev/ttyp5    0(m11)    &R.x
// 
//   |     |    |      |         |         |   |     | ¦___ window permissions
//   |     |    |      |         |         |   |     |      (R. is locked r-only,
//   |     |    |      |         |         |   |     |       W has wlock)
//   |     |    |      |         |         |   |     |
//   |     |    |      |         |         |   |     |___ Window is shared
//   |     |    |      |         |         |   |
//   |     |    |      |         |         |   |___ Name/Title of window
//   |     |    |      |         |         |
//   |     |    |      |         |         |___ Number of window
//   |     |    |      |         |
//   |     |    |      |         |___ Name of the display (the attached device)
//   |     |    |      |
//   |     |    |      |___ Username who is logged in at the display
//   |     |    |
//   |     |    |___ Display is in nonblocking mode. Shows 'NB' if obuf is full.
//   |     |
//   |     |___ Displays geometry as width x height.
//   |
//   |___ the terminal type known by screen for this display.

static int 
gl_display_header(list_data_t* ldata) {
    UNUSED(ldata);

    leftline("term-type   size         user interface           window       perms", 0, NULL);
    leftline("---------- ------- ---------- ----------------- ----------     -----", 1, NULL);

    return 2;
}

static int 
gl_display_footer(list_data_t* ldata) {
    UNUSED(ldata);

    centerline("[Press ctrl-l to refresh; Return to end.]", flayer->l_height - 1);

    return 1;
}

static int 
gl_display_row(list_data_t* ldata, list_row_t* lrow) {
    static char* blockstates[5] = { "nb", "NB", "Z<", "Z>", "BL" };
    char tbuf[80];

    display_t* D = lrow->data;
    window_t* win = D->d_fore;

    struct mchar m_current = mchar_blank;
    m_current.attr = A_BD;

    sprintf(
            tbuf, 
            " %-10.10s%4dx%-4d%10.10s@%-16.16s%s",
            D->d_termname, D->d_width, D->d_height, D->d_user->u_name,
            D->d_usertty,
            (D->d_blocked || D->d_nonblock >= 0) && D->d_blocked <= 4 ? blockstates[D->d_blocked] : "  "
            );

    if (win) {
        int len = 10 - strlen(win->w_title);

        if (len < 0)
            len = 0;

        char r_nest1 = (win->w_wlock == WLOCK_OFF || D->d_user == win->w_wlockuser) ? 'r' : 'R';
        char w_nest2 = D->d_user == win->w_wlockuser ? 'W' : 'v';
        char w_nest1 = (win->w_wlock == WLOCK_OFF) ? 'w' : w_nest2;

        sprintf(
                tbuf + strlen(tbuf), 
                "%3d(%.10s)%*s%c%c%c%c",
                win->w_number,
                win->w_title,
                len,
                "",
                /* w->w_dlist->next */ 0 ? '&' : ' ',
                // the rwx triple:
                // -,r,R      no read, read, read only due to foreign wlock
                // -,.,w,W    no write, write suppressed by foreign wlock, write, own wlock
                // -,x        no execute, execute
                (AclCheckPermWin(D->d_user, ACL_READ, win) ? '-' : r_nest1),
                (AclCheckPermWin(D->d_user, ACL_READ, win) ? '-' : w_nest1),
                (AclCheckPermWin(D->d_user, ACL_READ, win) ? '-' : 'x')
                );
    }

    leftline(tbuf, lrow->y, lrow == ldata->selected ? &mchar_so : D == display ? &m_current : NULL);

    return 1;
}

static int 
gl_display_rebuild(list_data_t* ldata) {
    // recreate the rows
    display_t* D;
    list_row_t *row = NULL;

    if (flayer->l_width < 10 || flayer->l_height < 5)
        return -1;

    for (D = displays; D; D = D->d_next) {
        row = glist_add_row(ldata, D, row);

        if (D == display)
            ldata->selected = row;
    }

    glist_display_all(ldata);

    return 0;
}

static int 
gl_display_input(list_data_t* ldata, char** inputp, size_t* len) {
    display_t* curr_D = display;
    unsigned char ch;

    if (!ldata->selected)
        return 0;

    ch = (unsigned char) **inputp;
    ++*inputp;
    --*len;

    switch (ch) {
        case '\f': // ^L to refresh
            glist_remove_rows(ldata);
            gl_display_rebuild(ldata);
            break;
        case '\r':
        case '\n':
            glist_abort();
            *len = 0;
            break;
        case 'd': // detach
        case 'D': // power detach
            display = ldata->selected->data;

            if (display == curr_D) // we do not allow detaching the current display
                break;

            Detach(ch == 'D' ? D_REMOTE_POWER : D_REMOTE);

            display = curr_D;

            glist_remove_rows(ldata);
            gl_display_rebuild(ldata);
            break;
        default:
            // we didn't actually process the input.
            --*inputp;
            ++*len;
            return 0;
    }

    return 1;
}

static int 
gl_display_freerow(list_data_t* ldata, list_row_t* lrow) {
    UNUSED(ldata);
    UNUSED(lrow);
    //
    // there was no allocation when row->data was set. So nothing to do here.
    return 0;
}

static int 
gl_display_free(list_data_t *ldata) {
    UNUSED(ldata);

    // there was no allocation in ldata->data. So nothing to do here
    return 0;
}

static const generic_list_t 
gl_display = {
    gl_display_free,
    gl_display_freerow,
    NULL, // we do not allow searching in the display list, at the moment
    gl_display_input,
    gl_display_footer,
    gl_display_header,
    gl_display_row,
    gl_display_rebuild,
};

void 
display_displays(void) {
    list_data_t* ldata;

    if (flayer->l_width < 10 || flayer->l_height < 5) {
        LMsg(0, "Window size too small for displays page");
        return;
    }

    ldata = glist_display(&gl_display, listid);

    if (!ldata)
        return;

    gl_display_rebuild(ldata);
}
