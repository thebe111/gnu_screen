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

#include "include/backtick.h"
#include "include/fileio.h"
#include "include/misc.h"
#include "include/winmsg.h"

/* TODO: get rid of global var */
backtick_t* backticks;

static void 
backtick_filter(backtick_t* bt) {
	char *p, *q;
	int c;

	for (p = q = bt->result; (c = (unsigned char) *p++) != 0;) {
		if (c == '\t')
			c = ' ';

		if (c >= ' ' || c == '\005')
			*q++ = c;
	}

	*q = 0;
}

static void 
backtick_fn(event_t* event, void* data) {
	int i, j, k, l;
	backtick_t* bt = (backtick_t*) data;

	i = bt->bufi;
	l = read(event->fd, bt->buf + i, MAXSTR - i);

	if (l <= 0) {
		evdeq(event);
		close(event->fd);
		event->fd = -1;
		return;
	}

	i += l;

	for (j = 0; j < l; j++) {
		if (bt->buf[i - j - 1] == '\n')
			break;
    }

	if (j < l) {
		for (k = i - j - 2; k >= 0; k--) {
			if (bt->buf[k] == '\n')
				break;
        }

		k++;
		memmove(bt->result, bt->buf + k, i - j - k);
		bt->result[i - j - k - 1] = 0;
		backtick_filter(bt);
		window_changed(NULL, WINESC_BACKTICK);
	}

	if (j == l && i == MAXSTR) {
		j = MAXSTR / 2;
		l = j + 1;
	}

	if (j < l) {
		if (j)
			memmove(bt->buf, bt->buf + i - j, j);

		i = j;
	}

	bt->bufi = i;
}

void 
setbacktick(int num, int lifespan, int tick, char** cmdv) {
	struct backtick **btp, *bt;
	char** v;

	for (btp = &backticks; (bt = *btp) != NULL; btp = &bt->next) {
		if (bt->num == num)
			break;
    }

	if (!bt && !cmdv)
		return;

	if (bt) {
		for (v = bt->cmdv; *v; v++)
			free(*v);

		free(bt->cmdv);

		if (bt->buf)
			free(bt->buf);

		if (bt->event.fd >= 0)
			close(bt->event.fd);

		evdeq(&bt->event);
	}

	if (bt && !cmdv) {
		*btp = bt->next;
		free(bt);
		return;
	}

	if (!bt) {
		bt = malloc(sizeof(backtick_t));

		if (!bt) {
			Msg(0, "%s", strnomem);
			return;
		}

		memset(bt, 0, sizeof(backtick_t));
		bt->next = NULL;
		*btp = bt;
	}

	bt->num = num;
	bt->tick = tick;
	bt->lifespan = lifespan;
	bt->bestbefore = 0;
	bt->result[0] = 0;
	bt->buf = NULL;
	bt->bufi = 0;
	bt->cmdv = cmdv;
	bt->event.fd = -1;

	if (bt->tick == 0 && bt->lifespan == 0) {
		bt->buf = malloc(MAXSTR);

		if (bt->buf == NULL) {
			Msg(0, "%s", strnomem);
			setbacktick(num, 0, 0, NULL);
			return;
		}

		bt->event.type = EVENT_READ;
		bt->event.fd = readpipe(bt->cmdv);
		bt->event.handler = backtick_fn;
		bt->event.data = (char*) bt;

		if (bt->event.fd >= 0)
			evenq(&bt->event);
	}
}

char* 
runbacktick(backtick_t* bt, int* tick, time_t now) {
	int f, i, l, j;
	time_t now2;

	if (bt->tick && (!*tickp || bt->tick < *tickp))
		*tickp = bt->tick;

	if ((bt->lifespan == 0 && bt->tick == 0) || now < bt->bestbefore)
		return bt->result;

	f = readpipe(bt->cmdv);

	if (f == -1)
		return bt->result;

	i = 0;

	while ((l = read(f, bt->result + i, ARRAY_SIZE(bt->result) - i)) > 0) {
		i += l;

		for (j = 1; j < l; j++) {
			if (bt->result[i - j - 1] == '\n')
				break;
        }

		if (j == l && i == ARRAY_SIZE(bt->result)) {
			j = ARRAY_SIZE(bt->result) / 2;
			l = j + 1;
		}

		if (j < l) {
			memmove(bt->result, bt->result + i - j, j);
			i = j;
		}
	}

	close(f);
	bt->result[ARRAY_SIZE(bt->result) - 1] = '\n';

	if (i && bt->result[i - 1] == '\n')
		i--;

	bt->result[i] = 0;
	backtick_filter(bt);
	(void) time(&now2);
	bt->bestbefore = now2 + bt->lifespan;

	return bt->result;
}

// locate a backtick by its id (number); returns NULL if no such backtick exists
backtick_t* 
backtick_find_byid(int num) {
	for (backtick_t* bt = backticks; bt; bt = bt->next) {
		if (bt->num == num)
			return bt;
	}

	return NULL;
}
