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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/config.h"
#include "include/list_generic.h"
#include "include/screen.h"
#include "include/help.h"
#include "include/misc.h"

static char ListID[] = "license";

static char license[] = {
    "Copyright (c) 2018-2020 Alexander Naumov, Amadeusz Slawinski\n"
    "Copyright (c) 2015-2017 Juergen Weigert, Alexander Naumov, Amadeusz Slawinski\n"
    "Copyright (c) 2010-2014 Juergen Weigert, Sadrul Habib Chowdhury\n"
    "Copyright (c) 2008-2009 Juergen Weigert, Michael Schroeder, Micah Cowan, Sadrul Habib Chowdhury\n"
    "Copyright (c) 1993-2007 Juergen Weigert, Michael Schroeder\n"
    "Copyright (c) 1987 Oliver Laumann\n"

    "\n"

    "This program is free software; you can redistribute it and/or modify it under"
    "the terms of the GNU General Public License as published by the Free"
    "Software Foundation; either version 3, or (at your option) any later"
    "version.\n"

    "\n"

    "This program is distributed in the hope that it will be useful, but WITHOUT"
    "ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or"
    "FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for"
    "more details.\n"

    "\n"

    "You should have received a copy of the GNU General Public License along with"
    "this program (see the file COPYING); if not, see"
    "<https://www.gnu.org/licenses/>, or contact Free Software Foundation, Inc., 51"
    "Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA.\n"

    "\n"

    "Send bugreports, fixes, enhancements, t-shirts, money, beer & pizza to"
    " screen-devel@gnu.org\n"
};

static int 
gl_license_header(list_data_t* ldata) {
    UNUSED(ldata);

    char s[75]; // 75 == is strlen("Screen version ") + sizeof(version[60]);

    snprintf(s, 75, "Screen version %s", version);
    centerline(s, 0);

    return 1;
}

static int 
gl_license_footer(list_data_t* ldata) {
    UNUSED(ldata);

    centerline("[Press Space for next page or Return to end]", flayer->l_height - 1);

    return 1;
}

static int 
gl_license_row(list_data_t* ldata, list_row_t* lrow) {
    UNUSED(ldata);

    char* line = calloc(sizeof(char), flayer->l_width + 1);
    char* start = (char*) lrow->data;
    char* lastspace = start;
    size_t linelen = 0;

    for (char* c = start; (*c != 0) && (*c != '\n'); c++) {
        linelen++;

        if (*c == ' ')
            lastspace = c;

        if (linelen >= (size_t) flayer->l_width) {
            linelen = lastspace - start;
            break;
        }
    }

    strncpy(line, start, linelen);
    leftline(line, lrow->y, NULL);
    free(line);

    return 1;
}

static int 
gl_license_rebuild(list_data_t* ldata) {
    // recreate the rows
    char *lastspace = NULL;
    list_row_t* row = NULL;
    size_t linelen = 0;

    if (flayer->l_width < 40 || flayer->l_height < 5)
        return -1; 

    for (char* c = license; *c != 0; c++) {
        if (linelen == 0)
            row = glist_add_row(ldata, c, row);

        if (*c == '\n') {
            linelen = 0;
            lastspace = NULL;
            continue;
        } else if (*c == ' ')
            lastspace = c;

        linelen++;

        if (linelen >= (size_t) flayer->l_width) {
            if (lastspace) {
                c = lastspace;
                lastspace = NULL;
                linelen = 0;
            }
        }
    }

    glist_display_all(ldata);

    return 0;
}

static int 
gl_license_input(list_data_t* ldata, char** input, size_t* len) {
    unsigned char ch;

    if (!ldata->selected)
        return 0;

    ch = (unsigned char) **input;

    ++*input;
    --*len;

    switch (ch) {
        case '\f': // ^L to refresh
            glist_remove_rows(ldata);
            gl_license_rebuild(ldata);
            break;
        case '\r':
        case '\n':
            glist_abort();
            *len = 0;
            break;
        default:
            // we didn't actually process the input.
            --*input;
            ++*len;
            return 0;
    }

    return 1;
}

static int 
gl_license_freerow(list_data_t *ldata, list_row_t *row) {
    UNUSED(ldata);
    UNUSED(row);

    // there was no allocation in ldata->data. so nothing to do here.
    return 0;
}

static int 
gl_license_free(list_data_t *ldata) {
    UNUSED(ldata);

    // there was no allocation in ldata->data. so nothing to do here.
    return 0;
}

static const generic_list_t 
gl_license = {
    gl_license_free,
    gl_license_freerow,
    NULL, // We do not allow searching in the license page, at the moment
    gl_license_input,
    gl_license_footer,
    gl_license_header,
    gl_license_row,
    gl_license_rebuild,
};

void 
display_license(void) {
    list_data_t *ldata;

    if (flayer->l_width < 40 || flayer->l_height < 5) {
        LMsg(0, "Window size too small for license page");

        return;
    }

    ldata = glist_display(&gl_license, ListID);

    if (!ldata)
        return;

    gl_license_rebuild(ldata);
}
