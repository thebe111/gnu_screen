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

#include "include/viewport.h"
#include "include/screen.h"

int 
rethink_display_viewports(void) {
	viewport_t *viewport, *vp_next;

    // free old viewports
	for (canvas_t* canvas = display->d_cvlist; canvas; canvas = canvas->c_next) {
		for (viewport = canvas->c_vplist; viewport; viewport = vp_next) {
			vp_next = viewport->v_next;
			memset((char*) viewport, 0, sizeof(viewport_t));
			free(viewport);
		}

		canvas->c_vplist = NULL;
	}

	display->d_vpxmin = -1;
	display->d_vpxmax = -1;

	for (canvas_t* canvas = display->d_cvlist; canvas; canvas = canvas->c_next) {
		if ((viewport = malloc(sizeof(viewport_t))) == NULL)
			return -1;

        *viewport = (viewport_t) {
            .v_canvas = canvas,
            .v_xs = canvas->c_xs,
            .v_ys = canvas->c_ys,
            .v_xe = canvas->c_xe,
            .v_ye = canvas->c_ye,
            .v_xoff = canvas->c_xoff,
            .v_yoff = canvas->c_yoff,
            .v_next = canvas->c_vplist,
        };

		canvas->c_vplist = viewport;

		if (canvas->c_xs < display->d_vpxmin || display->d_vpxmin == -1)
			display->d_vpxmin = canvas->c_xs;

		if (canvas->c_xe > display->d_vpxmax || display->d_vpxmax == -1)
			display->d_vpxmax = canvas->c_xe;
	}

	return 0;
}

void 
rethink_viewport_offsets(canvas_t* canvas) {
	for (viewport_t* viewport = canvas->c_vplist; viewport; viewport = viewport->v_next) {
		viewport->v_xoff = canvas->c_xoff;
		viewport->v_yoff = canvas->c_yoff;
	}
}
