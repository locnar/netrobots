/* Map drawing module.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "drawing.h"
#include "field.h"
#include "robotserver.h"

// static cairo_t *map_context;
static cairo_t *cr;

/* transforms degrees in to radians */
double degtorad(int degrees)
{
    double const radiants = degrees * M_PI / 180.0;
    return radiants;
}

void shot_animation(cairo_t *cr, double /*size*/, double /*direction*/, Cannon *cannon)
{
    // reduce the reload time by half of it so it draws the explosion and the flash for half the reload time
    int const time = cannon->timeToReload - RELOAD_RATIO / 2;

    if (time <= 0) {
        // if the gun is loaded don't paint anything
        return;
    }

    cairo_save(cr);
    cairo_pattern_t *pat;

    // translate to 1000-y, rather than y, because the graphics canvas has 0 at the top, and 1000 at the bottom.
    cairo_translate(cr, cannon->x, 1000 - cannon->y);

#if 0
    /* flash of the shot, should be translate to the robot's position.  */
    cairo_scale(cr, size, size);
    cairo_rotate(cr, direction);

    pat = cairo_pattern_create_linear(128, 64, 128, 128.0);
    cairo_pattern_add_color_stop_rgba(pat, 0, 1, 0, 0, time / (RELOAD_RATIO / 2.0));
    cairo_pattern_add_color_stop_rgba(pat, 1, 1, 1, 0, time / (RELOAD_RATIO / 2.0));
    cairo_set_source(cr, pat);

    cairo_move_to(cr, 32, 50);
    cairo_arc_negative(cr, 32, 32, 18, 90 * (M_PI / 180.0), 0);
    cairo_arc_negative(cr, 68, 32, 18, M_PI, 90 * (M_PI / 180.0));
    cairo_arc_negative(cr, 50, 50, 18, M_PI, 0);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);
#endif

    /* explosion*/
    cairo_arc(cr, 0, 0, 40, 0, 2*M_PI);
    pat = cairo_pattern_create_radial(0, 0, 10, 0, 0, 40);
    cairo_pattern_add_color_stop_rgba(pat, 0, 1, 0, 0, time / (RELOAD_RATIO / 2.0));
    cairo_pattern_add_color_stop_rgba(pat, 0.3, 1, 0.5, 0, time / (RELOAD_RATIO / 2.0));
    cairo_pattern_add_color_stop_rgba(pat, 0.6, 1, 0.2, 0, time / (RELOAD_RATIO / 2.0));
    cairo_set_source(cr, pat);

    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    cairo_restore(cr);
}

/* draws the cannon with the right orientation */
void draw_cannon(cairo_t *cr, double direction)
{
    double const x1 = -5; double const y1 = 51;
    double const x2 = -5; double const y2 = 19;
    double const x3 =  5; double const y3 = 19;
    double const x4 =  5; double const y4 = 51;

    cairo_save(cr);
    cairo_rotate(cr, direction);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);	
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_line_to(cr, x3, y3);
    cairo_line_to(cr, x4, y4);
    cairo_close_path(cr);

    cairo_fill(cr);
    cairo_restore(cr);
}

/* draws the radar inside the robot with the right orientation */
void draw_radar(cairo_t *cr, double direction)
{
    double const x1 =   0; double const y1 =  22;
    double const x2 =  20; double const y2 = -10;
    double const x3 = -20; double const y3 = -10;

    cairo_save(cr);
    cairo_rotate(cr, direction);
    cairo_set_source_rgba(cr, 1, 0, 0, 0.8);
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_line_to(cr, x3, y3);
    cairo_close_path(cr);

    cairo_fill(cr);
    cairo_restore(cr);
}

/* draws the name and a health bar for every robot (maybe in the future also the reloading animation */
void draw_stats(cairo_t *cr, Robot **all)
{
    int const space = 60;
    cairo_pattern_t *pat;

    cairo_save(cr);
    cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 0.2);
    cairo_rectangle(cr, 0, 0, 1100, 1100);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.5);
    cairo_rectangle(cr, 1, 1, 1100, 1078);
    cairo_stroke(cr);

    cairo_restore(cr);

    cairo_save(cr);
    cairo_translate(cr, 1320, 0);

    cairo_set_source_rgba(cr, 0.4, 0.4, 0.4, 0.2);
    cairo_rectangle(cr, 0, 0, 120, 540);
    cairo_fill(cr);

    for (int i = 0; i < max_robots; i++) {
        // display the name of the robot
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 14.0);
        cairo_set_source_rgba(cr, 0, 0, 0, 0.5);   // black?

        cairo_move_to(cr, 10.0, 15.0 + i * space);		
        cairo_show_text(cr, all[i]->name);

        // line with gradient from red to green
        cairo_move_to(cr, 10, 35 + i * space);
        cairo_set_line_width(cr, 30);
        cairo_line_to(cr, 110, 35 + i * space);
        pat = cairo_pattern_create_linear(100.0, 0.0,  0.0, 0.0);
        cairo_pattern_add_color_stop_rgba(pat, 1, 1, 0, 0, 1);
        cairo_pattern_add_color_stop_rgba(pat, 0, 0, 1, 0, 1);
        cairo_set_source(cr, pat);
        cairo_stroke(cr);
        cairo_pattern_destroy(pat);

        // black line on top of the colored one
        cairo_move_to(cr, 10, 35 + i * space);
        cairo_set_line_width(cr, 24);
        cairo_set_source_rgba(cr, 0, 0, 0, 0.5);
        cairo_line_to(cr, 110 - all[i]->damage, 35 + i * space);
        cairo_stroke(cr);

        // reloading animation goes here

        // separation line between the different robots
        cairo_move_to(cr, 20, 55 + i * space);
        cairo_set_line_width(cr, 4);
        cairo_set_source_rgb(cr, all[i]->color[0], all[i]->color[1], all[i]->color[2]);
        cairo_line_to(cr, 100, 55 + i * space);
        cairo_stroke(cr);
    }

    cairo_restore(cr);
}

/* draws a robot with a given size, using the various parameters(orientation, position,..) from the robot struct */
void draw_robot(cairo_t *cr, Robot *myRobot, double size)
{
    double const x1 = -70;
    double const y1 = -30;
    double const y2 =  10;
    double const x4 =  70;
    double const x5 =   0;
    double const y5 = -100;

    double const px2 = -70;
    double const py2 = 100;
    double const px3 =  70;
    double const py3 = 100;
    double const px4 =  70;
    double const py4 =  10;

    cairo_save(cr);

    // translate to 1000-y, rather than just y, because the graphics canvas has 0 at the top, and 1000 at the bottom.
    cairo_translate(cr, myRobot->x, 1000 - myRobot->y);
    cairo_scale(cr, size, size);

    cairo_save(cr);

    // translate to 90-degrees because the graphics canvas has 0 at the top, and 1000 at the bottom.
    cairo_rotate(cr, degtorad(90 - myRobot->degree));

    cairo_set_source_rgba(cr, myRobot->color[0], myRobot->color[1], myRobot->color[2], 0.6);
    cairo_set_line_width(cr, 2);
    cairo_move_to(cr,x1,y1);
    cairo_line_to(cr,x1,y2);
    cairo_curve_to(cr, px2, py2, px3, py3, px4, py4);
    cairo_line_to(cr, x4, y1);
    cairo_line_to(cr, x5, y5);
    cairo_close_path(cr);
    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
    cairo_move_to(cr, 30, 0);
    cairo_arc_negative(cr, 0, 0, 30, 0, -2 * M_PI);

    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 0.1, 0.7, 0.5, 0.5);	

    cairo_stroke(cr);

    cairo_restore(cr); /* pop rotate */

    // rotation amounts here must be "270-desireddirection"
    // because the display canvas has 0 at the top, and
    // larger numbers increasing downwards.
    // (These were previously 270+myRobot->xxx_degree).
    draw_cannon(cr, degtorad(270 - myRobot->cannon_degree));
    draw_radar(cr, degtorad(270 - myRobot->radar_degree));

    cairo_restore(cr); /* pop translate/scale */

    shot_animation(cr, size, degtorad(myRobot->cannon_degree), &myRobot->cannon[0]);
    shot_animation(cr, size, degtorad(myRobot->cannon_degree), &myRobot->cannon[1]);
}

void draw(cairo_t *cr)
{
    cairo_save(cr);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    cairo_scale(cr, WIN_HEIGHT / 1000.0, WIN_HEIGHT / 1000.0);
    for (int i = 0; i < max_robots; i++) {
        draw_robot(cr, all_robots[i], 0.4);
    }

    cairo_restore(cr);	

    draw_stats(cr, all_robots);

//  cairo_save(cairo_context);
//  cairo_set_operator(cairo_context, CAIRO_OPERATOR_SOURCE);
//  cairo_set_source_surface(cairo_context, cairo_get_target(map_context), 0, 0);
//  cairo_paint(cairo_context);
//  cairo_restore(cairo_context);
}

void update_display(SDL_Event *event)
{	
    draw(cr);

    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_q) {
            event->type = SDL_QUIT;
            SDL_PushEvent(event);
        }
    }
}

/* can be used to add a background to the window*/
void init_cairo()
{
    cr = init_sdl();
#if 0
    cairo_surface_t *png_map = cairo_image_surface_create_from_png("background.png");
    map_context = create_cairo_context();
    cairo_set_source_surface(map_context, png_map, 0, 0);
    cairo_paint(map_context);
    cairo_surface_destroy(png_map);
#endif
}

void destroy_cairo()
{
    destroy_cairo_context(cr);
    free_sdl();
}
