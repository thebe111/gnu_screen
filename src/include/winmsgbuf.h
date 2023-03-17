// Copyright (c) 2013
//      Mike Gerwitz (mtg@gnu.org)
//
// This file is part of GNU screen.
//
// GNU screen is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program (see the file COPYING); if not, see [0]
//
// [0]: <https://www.gnu.org/licenses>

#ifndef SCREEN_WINMSGBUF_H
#define SCREEN_WINMSGBUF_H

#include <stddef.h>
#include <stdint.h>
#include "screen.h"

// default window message buffer size
#define WINMSG_BUF_SIZE MAXSTR // MAXSTR coming from screen.h
// rendition changes
#define MAX_WINMSG_REND 256 

// TODO: complete truncation and rendition API

// represents a working buffer for window messages
typedef struct winmsg_buf_s {
    char* buf;
    size_t size;
    uint64_t rend[MAX_WINMSG_REND];
    int rendpos[MAX_WINMSG_REND];
    int numrend;
} winmsg_buf_t;

typedef struct winmsg_buf_context_s {
	winmsg_buf_t *buf;
    // pointer within buffer
	char* p;

    // truncation mark
	struct {
		// starting position of truncation;
        // TODO: make this size_t and remove 1 as inactive indicator
		int pos;
        // target offset percentage relative to pos and trunc operator
		uint8_t perc;
        // whether to show ellipses
		bool ellip;
	} trunc;
} winmsg_buf_context_t;

__attribute__((format(printf,2,3))) int wmbc_printf(winmsg_buf_context_t* ctx, const char* fmt, ...);
const char* wmb_contents(const winmsg_buf_t* win);
const char* wmbc_finish(winmsg_buf_context_t* ctx);
const char* wmbc_mergewmb(winmsg_buf_context_t* ctx, winmsg_buf_t* win);
const char* wmbc_strcpy(winmsg_buf_context_t* ctx, const char *s);
const char* wmbc_strncpy(winmsg_buf_context_t* ctx, const char *s, size_t size);
size_t wmb_expand(winmsg_buf_t* in, size_t size);
size_t wmb_size(const winmsg_buf_t* win);
size_t wmbc_bytesleft(winmsg_buf_context_t* ctx);
size_t wmbc_offset(winmsg_buf_context_t* ctx);
void wmb_free(winmsg_buf_t* win);
void wmb_rendadd(winmsg_buf_t* win, uint64_t render, int offset);
void wmb_reset(winmsg_buf_t* win);
void wmbc_fastfw0(winmsg_buf_context_t* ctx);
void wmbc_fastfw_end(winmsg_buf_context_t* ctx);
void wmbc_free(winmsg_buf_context_t* ctx);
void wmbc_putchar(winmsg_buf_context_t* ctx, char ch);
void wmbc_rewind(winmsg_buf_context_t* ctx);
winmsg_buf_context_t *wmbc_create(winmsg_buf_t* buf);
winmsg_buf_t* wmb_create(void);

#endif // SCREEN_WINMSGBUF_H
