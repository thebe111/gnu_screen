/* Copyright (c) 2013
 *      Mike Gerwitz (mtg@gnu.org)
 * Copyright (c) 2010
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

#ifndef SCREEN_BACKTICK_H
#define SCREEN_BACKTICK_H

#include <time.h>
#include "screen.h"

typedef struct backtick_s {
    event_t event;
    char** cmdv;
    char* buf;
    char result[MAXSTR];  /* TODO: not re-entrant */
    int bufi;
    int lifespan;
    int num;
    int tick;
    struct backtick* next;
    time_t bestbefore;
} backtick_t;

// TODO: these still need refactoring
void setbacktick(int num, int lifespan, int tick, char** cmdv);
char* runbacktick(backtick_t* bt, int* tick, time_t now);

// opaque interface
backtick_t* backtick_find_byid(int);

#endif // SCREEN_BACKTICK_H
