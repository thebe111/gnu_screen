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
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include "include/config.h"
#include "include/sched.h"
#include "include/screen.h"

static event_t* events;
static event_t* next_event;
static event_t* timeout_events;
static int calctimeout;
static int pfd_cnt;
static struct pollfd* pfd;

static event_t*
calc_timeout(void) {
    event_t *event, *min;
    long mins;

    if ((min = timeout_events) == NULL)
        return NULL;

    mins = min->timeout;

    for (event = timeout_events->next; event; event = event->next) {
        if (mins > event->timeout) {
            min = event;
            mins = event->timeout;
        }
    }

    return min;
}

void 
sched(void) {
    event_t* event;
    event_t* timeout_event = NULL;
    int i, n;
    int timeout;

    for (;;) {
        if (calctimeout)
            timeout_event = calc_timeout();

        if (timeout_event) {
            struct timeval now;

            gettimeofday(&now, NULL);
            timeout = timeout_event->timeout - (now.tv_sec * 1000 + now.tv_usec / 1000);

            if (timeout < 0)
                timeout = 0;
        }

        memset(pfd, 0, sizeof(struct pollfd) * pfd_cnt);
        i = 0;

        for (event = events; event; event = event->next) {
            if (event->condpos && *event->condpos <= (event->condneg ? *event->condneg : 0))
                goto skip;

            if (event->type == EVENT_READ) {
                pfd[i].fd = event->fd;
                pfd[i].events = POLLIN;
            } else if (event->type == EVENT_WRITE) {
                pfd[i].fd = event->fd;
                pfd[i].events = POLLOUT;
            }
skip:
            if (event->type == EVENT_READ || event->type == EVENT_WRITE)
                i++;
        }

        n = poll(pfd, i, timeout_event ? timeout : 1000);

        if (n < 0) {
            if (errno != EINTR)
                Panic(errno, "poll");

            n = 0;
        } else if (n == 0) {
            // timeout

            if (timeout_event) {
                event_dequeue(timeout_event);
                timeout_event->handler(timeout_event, timeout_event->data);
            }
        }

        i = 0;

        for (event = events; event; event = next_event) {
            next_event = event->next;

            switch (event->type) {
                case EVENT_READ:
                case EVENT_WRITE:
                    // check if we parsed all events from poll() if we did just
                    // continue, as we may still need to run EVENT_ALWAYS event
                    if (n == 0)
                        continue;

                    // check if we have anything to do for EVENT_READ or
                    // EVENT_WRITE, or if event is still queued, if not skip to
                    // the next event
                    if (!pfd[i].revents || pfd[i].fd < 0) {
                        i++;
                        continue;
                    }

                    // this is one of events from poll(), decrease counter 
                    n--;

                    // advance pollfd pointer
                    i++;
                    __attribute__ ((fallthrough));
                default:
                    if (event->condpos && *event->condpos <= (event->condneg ? *event->condneg : 0))
                        continue;

                    event->handler(event, event->data);
                    break;
            }
        }
    }
}

void 
set_timeout(event_t* event, int timeout) {
    struct timeval now;
    
    gettimeofday(&now, NULL);
    event->timeout = (now.tv_sec * 1000 + now.tv_usec / 1000) + timeout;

    if (event->queued)
        calctimeout = 1;
}

void 
event_enqueue(event_t* event) {
    int i = 0;
    event_t *eventp, **eventpp;

    if (event->queued)
        return;

    eventpp = &events;

    if (event->type == EVENT_TIMEOUT) {
        calctimeout = 1;
        eventpp = &timeout_events;
    }

    for (; (eventp = *eventpp); eventpp = &eventp->next) {
        if (event->priority > eventp->priority)
            break;
    }

    event->next = eventp;
    *eventpp = event;
    event->queued = true;

    // check if we need more pollfd
    for (eventp = events; eventp; eventp = eventp->next) {
        if (eventp->type == EVENT_READ || eventp->type == EVENT_WRITE)
            i++;
    }

    if (i > pfd_cnt) {
        pfd_cnt = i;
        pfd = realloc(pfd, pfd_cnt * sizeof(struct pollfd));
    }
}

void 
event_dequeue(event_t* event) {
    event_t *eventp, **eventpp;

    if (!event || !event->queued)
        return;

    eventpp = &events;

    if (event->type == EVENT_TIMEOUT) {
        calctimeout = 1;
        eventpp = &timeout_events;
    }

    for (; (eventp = *eventpp); eventpp = &eventp->next) {
        if (eventp == event)
            break;
    }

    *eventpp = event->next;
    event->queued = false;

    if (event == next_event)
        next_event = next_event->next;

    // mark fd to be skipped (see checks in sched())
    for (int i = 0; i < pfd_cnt; i++) {
        if (pfd[i].fd == event->fd)
            pfd[i].fd = -pfd[i].fd;
    }
}
