/* Copyright (c) 2013
 *      Mike Gerwitz (mtg@gnu.org)
 *
 * This file is part of GNU screen.
 *
 * GNU screen is free software; you can redistribute it and/or modify
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
 * <https://www.gnu.org/licenses>.
 *
 ****************************************************************
 */

#include <assert.h>
#include <stdlib.h>
#include "include/config.h"
#include "include/winmsgcond.h"

// determine if condition is active (has been initialized and can be used)
bool 
wmc_is_active(const winmsg_cond_t* cond) {
    return cond->initialized;
}

// determine if a condition is true; the result is undefined if !wmc_active(cond) 
bool 
wmc_is_set(const winmsg_cond_t* cond) {
    return cond->state;
}

// "else" encounted
int 
wmc_else(winmsg_cond_t* cond, int offset, bool* changed) {
    assert(wmc_is_active(cond));

    // if we're already set, then there is no point in processing the "else";
    // we will therefore consider the previous condition to have succeeded, so
    // now keep track of the start of the else so that it may be clobbered
    // instead of the beginning of the true condition - that is, we're accepting
    // the destination string up until this point 
    if (wmc_is_set(cond)) {
        wmc_init(cond, offset);     // track this as a new condition
        cond->locked = true;        // "else" shall never succeed at this point

        // we want to keep the string we have so far (the truth string)
        if (changed)
            *changed = false;

        return offset;
    }

    // now that we have reached "else" and are not true, we can never be true;
    // discard the truth part of the string
    int prev_off = cond->offset;
    cond->offset = offset;

    // the "else" part must always be true at this point, because the previous
    // condition failed
    wmc_set(cond);
    cond->locked = true;

    if (changed)
        *changed = true;

    return prev_off;
}

// end condition and determine if string should be reset or kept - if our value
// is truthful, then accept the string, otherwise reject and reset to the
// position that we were initialized with
int 
wmc_end(const winmsg_cond_t* cond, int offset, bool* changed) {
    bool set = wmc_is_set(cond);

    if (changed)
        *changed = !set;

    return set ? offset : cond->offset;
}

// clear condition (equivalent to non-match)
void 
wmc_clear(winmsg_cond_t* cond) {
    if (cond->locked)
        return;

    cond->state = false;
}

// deactivate a condition, preventing its use; this allows a single allocation
// to be re-used and ignored until activated
void 
wmc_deinit(winmsg_cond_t* cond) {
    cond->state = false;
    cond->offset = 0;
    cond->locked = true;
    cond->initialized = false;
}

// initialize new condition and set to false; can be used to re-initialize a
// condition for re-use
void 
wmc_init(winmsg_cond_t* cond, int offset) {
    cond->locked = false;
    cond->offset = offset;
    cond->initialized = true;
    wmc_clear(cond);
}

// mark condition as true
void 
wmc_set(winmsg_cond_t* cond) {
    if (cond->locked)
        return;

    cond->state = true;
}
