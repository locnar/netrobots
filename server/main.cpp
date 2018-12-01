/* Main program.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
        /* jag; 12nov2018 begin  added lines */
#include <time.h>
#include <unistd.h>
        /* jag; 12nov2018 end of added lines */

#include "drawing.h"
#include "field.h"
#include "net_defines.h"

        /* jag; 12nov2018 begin  added lines */
int server_init( int argc, char *argv[] );
int server_cycle ( SDL_Event *event );
void destroy_cairo( );
        /* jag; 12nov2018 end of added lines */

int
main (int argc, char **argv)
{
  unsigned int i = 0, start_ticks, end_ticks;

  /* initialize SDL and create as OpenGL-texture source */
  //  cairo_t *cairo_context;
  srandom(time(NULL) + getpid());
  server_init(argc, argv);
  init_cairo();

  start_ticks = SDL_GetTicks ();
	
  /* enter event-loop */
  for (;;) {
	SDL_Event event;
	i++;
	draw_sdl ();
	event.type = -1;
	SDL_PollEvent (&event);

	/* check for user hitting close-window widget */
	if (event.type == SDL_QUIT)
		break;

	/* Call functions here to parse event and render on cairo_context...  */
	if ( 1 == server_cycle(&event) ) {
            break;        // if server_cycle returns 1 then match is over
        }
    }
	
  printf( "Total match time: %.2f seconds; ", (double)(SDL_GetTicks () - start_ticks)/1000 );

  end_ticks = SDL_GetTicks();
  if ( start_ticks != end_ticks ) {
    printf ("frame rate: %.2f fps\n", (i * 1000.0) / (end_ticks - start_ticks));
  }
  sleep( 2 );  // let spectators view the final moment for 2 seconds,
               // before clearing the arena

  /* clear resources before exit */
  destroy_cairo();

  return 0;
}
