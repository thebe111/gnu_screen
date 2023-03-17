/* Copyright (c) 2010
 *      Juergen Weigert (jnweiger@immd4.informatik.uni-erlangen.de)
 *      Sadrul Habib Chowdhury (sadrul@users.sourceforge.net)
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

#ifndef SCREEN_LIST_GENERIC_H
#define SCREEN_LIST_GENERIC_H

#include <stdlib.h>
#include "window.h"

typedef struct list_row_s {
    list_row_s *next, *prev;    // doubly linked list
    int y;                      // -1 if not on display
    void *data;                 // Some data relevant to this row
} list_row_t;

typedef struct list_data_s list_data_t;

typedef struct generic_list_s {
    int (*gl_free) (list_data_t* ldata);                                        // free data for the list
    int (*gl_freerow) (list_data_t* ldata, list_row_t* lrow);                   // free data for a row
    int (*gl_matchrow) (list_data_t* ldata, list_row_t* lrow, const char*);  
    int (*gl_pinput) (list_data_t* ldata, char** inp, size_t* len);             // process input
    int (*gl_pfooter) (list_data_t* ldata);                                 // print the footer
    int (*gl_pheader) (list_data_t* ldata);                                 // print the header
    int (*gl_prow) (list_data_t* ldata, list_row_t* lrow);                  // print one row
    int (*gl_rebuild) (list_data_t* ldata);                                     // rebuild display
} generic_list_t;

struct list_data_s {
    list_row_t* root;               // The first item in the list
    list_row_t* selected;           // The selected row
    list_row_t* top;                // The topmost visible row
    char* search;                   // The search term, if any
    const generic_list_t* list_fn;  // The functions that deal with the list
    const char* name;               // An identifier for the list
    void* data;                     // List specific data
};

list_data_t* glist_display (const generic_list_t* list, const char* name);
list_row_t* glist_add_row (list_data_t* ldata, void* data, list_row_t* after);
void display_displays (void);
void display_license (void);
void display_windows (int onblank, int order, Window* group);
void glist_abort (void);
void glist_display_all (list_data_t* ldata);
void glist_remove_rows (list_data_t* ldata);

/* == GLOBAL VARIABLES == */

extern const struct LayFuncs ListLf;

#endif // SCREEN_LIST_GENERIC_H
