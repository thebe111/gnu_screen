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

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include "include/config.h"
#include "include/winmsgbuf.h"

// attempts buffer expansion and updates context pointers appropriately. The
// result is true if expansion succeeded, otherwise false
static bool 
_wmbc_expand(winmsg_buf_context_t* ctx, size_t size) {
    size_t offset = wmbc_offset(ctx);

    if (wmb_expand(ctx->buf, size) < size)
        return false;

    // the buffer address may have changed; re-calculate pointer address
    ctx->p = ctx->buf->buf + offset;

    return true;
}

// write data to the buffer using a printf-style format string. If needed, the
// buffer will be automatically expanded to accomodate the resulting string and
// is therefore protected against overflows
int 
wmbc_printf(winmsg_buf_context_t* ctx, const char* fmt, ...) {
    va_list ap;
    size_t  len, max;

    // to prevent buffer overflows, cap the number of bytes to the remaining
    // buffer size
    va_start(ap, fmt);
    max = wmbc_bytesleft(ctx);
    len = vsnprintf(ctx->p, max, fmt, ap);
    va_end(ap);

    // more space is needed if vsnprintf returns a larger number than our max,
    // in which case we should accomodate by dynamically resizing the buffer and
    // trying again
    if (len > max) {
        if (!_wmbc_expand(ctx, wmb_size(ctx->buf) + len - max)) {
            // failed to allocate additional memory; this will simply have to do
            wmbc_fastfw_end(ctx);
            return max;
        }

        va_start(ap, fmt);
        size_t m = vsnprintf(ctx->p, len + 1, fmt, ap);
        assert(m == len); // this should never fail
        va_end(ap);
    }

    wmbc_fastfw0(ctx);

    return len;
}

// retrieve a pointer to the raw buffer contents. This should not be used to
// modify the buffer. */
const char*
wmb_contents(const winmsg_buf_t* win) {
    return win->buf;
}

// write a terminating null byte to the buffer and return a pointer to the
// buffer contents. This should not be used to modify the buffer. If buffer is
// full and expansion fails, then the last byte in the buffer will be replaced
// with the null byte
const char* 
wmbc_finish(winmsg_buf_context_t* ctx) {
    if (!wmbc_bytesleft(ctx)) {
        size_t size = ctx->buf->size + 1;

        if (wmb_expand(ctx->buf, size) < size)
            // we must terminate the string or we may cause big problems for the
            // caller; overwrite the last char :x
            ctx->p--;
    }

    *ctx->p = '\0';

    return wmb_contents(ctx->buf);
}

// merges the contents of another null-terminated buffer and its renditions. The
// return value is a pointer to the first character of WMBs buffer
const char* 
wmbc_mergewmb(winmsg_buf_context_t* ctx, winmsg_buf_t* win) {
    const char *p;
    size_t offset = wmbc_offset(ctx);

    // import buffer contents into our own at our current position
    assert(win);

    p = wmbc_strcpy(ctx, win->buf);

    // merge renditions, adjusting them to reflect their new offset
    for (int i = 0; i < win->numrend; i++) 
        wmb_rendadd(ctx->buf, win->rend[i], offset + win->rendpos[i]);

    return p;
}

// copies a string into the buffer, dynamically resizing the buffer as needed to
// accomodate the length of the string sans its terminating null byte. The
// context pointer is adjusted to the the terminiating null byte. A pointer to
// the first copied character in the destination buffer is returned; it shall
// not be used to modify the buffer
const char* 
wmbc_strcpy(winmsg_buf_context_t* ctx, const char* s) {
    return wmbc_strncpy(ctx, s, strlen(s));
}

// copies a string into the buffer, dynamically resizing the buffer as needed to
// accomodate length N. If S is shorter than N characters in length, the
// remaining bytes are filled will nulls. The context pointer is adjusted to the
// terminating null byte. A pointer to the first copied character in the buffer
// is returned; it shall not be used to modify the buffer
const char* 
wmbc_strncpy(winmsg_buf_context_t* ctx, const char* s, size_t len) {
    size_t l = wmbc_bytesleft(ctx);

    // silently fail in the event that we cannot accomodate
    if (l < len) {
        size_t size = ctx->buf->size + (len - l);

        if (!_wmbc_expand(ctx, size))
            // TODO: we should copy what can fit
            return NULL;
    }

    char *p = ctx->p;

    strncpy(ctx->p, s, len);
    ctx->p += len;

    return p;
}

// attempts to expand the buffer to hold at least MIN bytes. The new size of the
// buffer is returned, which may be unchanged from the original size if
// additional memory could not be allocated
size_t 
wmb_expand(winmsg_buf_t* win, size_t min) {
    size_t size = win->size;

    if (size >= min)
        return size;

    // keep doubling the buffer until we reach at least the requested size; this
    // ensures that we'll always be a power of two (so long as the original
    // buffer size was) and doubling will help cut down on excessive allocation
    // requests on large buffers
    while (size < min)
        size *= 2;

    void* p = realloc(win->buf, size);

    if (p == NULL)
        // reallocation failed; maybe the caller can do without?
        return win->size;

    // realloc already handled the free for us
    win->buf = p;
    win->size = size;

    return size;
}

// retrieve buffer size. This returns the total size of the buffer, not how much
// has been used
size_t 
wmb_size(const winmsg_buf_t* win) {
    return win->size;
}

// calculate the number of bytes remaining in the buffer relative to the current
// position within the buffer
size_t 
wmbc_bytesleft(winmsg_buf_context_t* ctx) {
    return ctx->buf->size - wmbc_offset(ctx);
}

// retrieve the 0-indexed offset of the context pointer into the buffer
size_t 
wmbc_offset(winmsg_buf_context_t* ctx) {
    ptrdiff_t offset = ctx->p - ctx->buf->buf;

    // when using wmbc_* functions (as one always should), the offset should
    // always be within the bounds of the buffer or one byte outside of it
    // (the latter case would require an expansion before writing)
    assert(offset > -1);
    assert((size_t) offset <= ctx->buf->size);

    return (size_t) offset;
}

// deinitialize and free memory allocated to the given window buffer
void 
wmb_free(winmsg_buf_t* win) {
    free(win->buf);
    free(win);
}

// add a rendition to the buffer
void 
wmb_rendadd(winmsg_buf_t* win, uint64_t render, int offset) {
    // TODO: lift arbitrary limit; dynamically allocate
    if (win->numrend >= MAX_WINMSG_REND)
        return;

    win->rend[win->numrend] = render;
    win->rendpos[win->numrend] = offset;
    win->numrend++;
}

// initializes window buffer to the empty string; useful for re-using an
// existing buffer without allocating a new one
void 
wmb_reset(winmsg_buf_t* win) {
    *win->buf = '\0';
    win->numrend = 0;
}

// place pointer at terminating null character
void 
wmbc_fastfw0(winmsg_buf_context_t* ctx) {
    ctx->p += strlen(ctx->p);
}

// place pointer just past the last byte in the buffer, ignoring terminating null
// characters. The next write will trigger an expansion. */
void 
wmbc_fastfw_end(winmsg_buf_context_t* ctx) {
    ctx->p = ctx->buf->buf + ctx->buf->size;
}

// deinitializes and frees previously allocated context. The contained buffer
// must be freed separately; this function will not do so for you
void 
wmbc_free(winmsg_buf_context_t* ctx) {
    free(ctx);
}

// sets a character at the current buffer position and increments the pointer.
// The terminating null character is not retained. The buffer will be
// dynamically resized as needed
void 
wmbc_putchar(winmsg_buf_context_t* ctx, char ch) {
    // attempt to accomodate this character, but bail out silenty if it cannot
    // fit
    if (!wmbc_bytesleft(ctx)) {
        if (!_wmbc_expand(ctx, ctx->buf->size + 1))
            return;
    }

    *ctx->p++ = ch;
}

// rewind pointer to the first byte of the buffer
void 
wmbc_rewind(winmsg_buf_context_t* ctx) {
    ctx->p = ctx->buf->buf;
}

// allocate and initialize a buffer context for the given buffer. The return
// value must be freed using wmbc_free
winmsg_buf_context_t* 
wmbc_create(winmsg_buf_t* win) {
    if (win == NULL)
        return NULL;

    winmsg_buf_context_t* ctx = malloc(sizeof(winmsg_buf_context_t));

    if (ctx == NULL)
        return NULL;

    ctx->buf = win;
    ctx->p = win->buf;

    return ctx;
}

// allocate and initialize to the empty string a new window message buffer. The
// return value must be freed using wmbc_free
winmsg_buf_t*
wmb_create(void) {
    winmsg_buf_t* win = malloc(sizeof(winmsg_buf_t));

    if (win == NULL)
        return NULL;

    win->buf = malloc(WINMSG_BUF_SIZE);

    if (win->buf == NULL) {
        free(win);
        return NULL;
    }

    win->size = WINMSG_BUF_SIZE;
    wmb_reset(win);

    return win;
}
