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
 * $Id$ GNU
 */

#ifndef SCREEN_SCHED_H
#define SCREEN_SCHED_H

#include <stdbool.h>
#include <sys/time.h>

typedef enum event_type_e {
    EVENT_TIMEOUT = 0,
    EVENT_READ = 1,
    EVENT_WRITE = 2,
    EVENT_ALWAYS = 3
} event_type_t;

typedef struct event_s {
    event_s* next;
    event_type_t type;
    bool queued;           // in events queue
    int* condneg;  
    int* condpos;          // only active if (condpos - condneg > 0)
    int fd;        
    int priority;  
    int timeout;           // timeout in milliseconds
    void (*handler)(event_s* event, void*);
    void* data;
} event_t;

__attribute__((__noreturn__)) void sched(void);
void set_timeout(event_t* event, int timeout);
void event_dequeue(event_t* event);
void event_enqueue(event_t* event);

#endif // SCREEN_SCHED_H
