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

#ifndef SCREEN_WINMSGCOND_H
#define SCREEN_WINMSGCOND_H

#include <stdbool.h>

// represents a window message condition (e.g. %?)
typedef struct {
	int  offset;       // offset in dest string
	bool initialized;  
	bool state;        // conditional truth value
	bool locked;       // when set, prevents state from changing
} winmsg_cond_t;

// winmsg_cond_t is intended to be used as an opaque type
bool wmc_is_active(const winmsg_cond_t* cond);
bool wmc_is_set(const winmsg_cond_t* cond);
int wmc_else(winmsg_cond_t* cond, int offset, bool* changed);
int wmc_end(const winmsg_cond_t* cond, int offset, bool* changed);
void wmc_clear(winmsg_cond_t* cond);
void wmc_deinit(winmsg_cond_t* cond);
void wmc_init(winmsg_cond_t* cond, int offset);
void wmc_set(winmsg_cond_t* cond);

#endif // SCREEN_WINMSGCOND_H
