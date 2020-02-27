/* Interface between Cairo and SDL+OpenGL.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#ifndef DRAWING_H
#define DRAWING_H

#include <cairo.h>
#include "SDL.h"

#define WIN_WIDTH    1440 //720
#define WIN_HEIGHT   1080 //540
#define WIN_TITLE    "Netrobots Battlefield"

// Functions to create more cairo contexts.
extern cairo_t* create_cairo_context();
extern void destroy_cairo_context(cairo_t *);

// Functions used by main.c as a high-level interface with SDL.
extern cairo_t* init_sdl();
extern void free_sdl();
extern void draw_sdl();

#endif /* DRAWING_H */
