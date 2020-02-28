/* Main program.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "drawing.h"
#include "field.h"
#include "net_defines.h"

int server_init(int argc, char **argv);
int server_cycle(SDL_Event *event);
void destroy_cairo( );

int main(int argc, char **argv)
{
    // initialize SDL and create as OpenGL-texture source
//  cairo_t *cairo_context;
    srandom(time(nullptr) + getpid());
    server_init(argc, argv);
    init_cairo();

    unsigned int const start_ticks = SDL_GetTicks ();

    // enter event-loop
    unsigned int frame = 0;
    for (;;) {
        SDL_Event event;
        frame++;
        draw_sdl();
        event.type = -1;
        SDL_PollEvent(&event);

        // check for user hitting close-window widget
        if (event.type == SDL_QUIT) {
            break;
        }

        // Call functions here to parse event and render on cairo_context...
        if (server_cycle(&event) == 1) {
            // if server_cycle returns 1 then match is over
            break;
        }
    }

    printf("Total match time: %.2f seconds; ", static_cast<double>(SDL_GetTicks () - start_ticks)/1000);

    unsigned int const end_ticks = SDL_GetTicks();
    if (start_ticks != end_ticks) {
        printf("frame rate: %.2f fps\n", (frame * 1000.0) / (end_ticks - start_ticks));
    }
    sleep(5);  // let spectators view the final moment for 5 seconds, before clearing the arena

    // clear resources before exit
    destroy_cairo();

    return 0;
}
