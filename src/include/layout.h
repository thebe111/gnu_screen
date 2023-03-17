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

#ifndef SCREEN_LAYOUT_H
#define SCREEN_LAYOUT_H

#include "canvas.h"

#define MAXLAY 10

typedef struct Layout {
	Layout *lay_next;
	canvas_t *lay_cvlist;
	canvas_t *lay_forecv;
	canvas_t lay_canvas;
	char *lay_title;
	int lay_autosave;
	int lay_number;
} Layout;

Layout* CreateLayout (char*, int);
Layout* CreateLayout(char*, int);
Layout* FindLayout (char*);
int LayoutDumpcanvas_t (canvas_t* canvas, char*);
int RenumberLayout (Layout* lay, int);
void AutosaveLayout (Layout* lay);
void FreeLayoutCv(canvas_t* canvas);
void LoadLayout (Layout* lay);
void NewLayout (char *, int);
void RemoveLayout (Layout* lay);
void SaveLayout (char*, canvas_t* canvas);
void ShowLayouts (int);
void UpdateLayoutcanvas_t (canvas_t* canvas, Window* win);
void RenameLayout (Layout*, const char*);

/* == GLOBAL VARIABLES == */

extern Layout *layout_attach, *layout_last, layout_last_marker;
extern Layout *layouts;

#endif // SCREEN_LAYOUT_H
