#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "net_utils.h"
#include "net_defines.h"
#include "robotserver.h"
#include "drawing.h"

#define STD_CLIENTS 5
#define STD_CYCLES 10000
#define STD_HOSTNAME NULL
#define STD_PORT "4300"

int current_robots = 0;
struct pollfd *fds;

extern int debug;
extern int max_robots;

int max_cycles = 0;
int current_cycles = 0;

float get_rand_color() {
    float color = static_cast<float>(random() / static_cast<float>(RAND_MAX));
    if (color < 0.1) {
        color = 0.1;
    }
    else if (color > 0.9) {
        color = 0.9;
    }
    return color;
}

static int quad = 0;

void update_display(SDL_Event * event);

int create_client(int fd)
{
	struct robot *r;

	if (fd == -1)
		return 0;
	if (!(r = (struct robot *) malloc (sizeof(struct robot))))
		return 0;
	memset (r, 0, sizeof (*r));

	/* place each robot in a different quadrant.  */
	r->x = ((quad & 1) ? 0 : 500) + 500 * (random() / (double) RAND_MAX);
	r->y = ((quad & 2) ? 0 : 500) + 500 * (random() / (double) RAND_MAX);
	quad++;

	r->color[0] = get_rand_color();
	r->color[1] = get_rand_color();
	r->color[2] = get_rand_color();

	fds[current_robots].fd = fd;
	all_robots[current_robots++] = r;

// jag; get the robot name, which the robot sends as the first string of
//      text, ending with a NUL ('\0', or 0).
        // the amount requested by the below read() call should be
        // STD_BUF bytes, to match the write() in robots.c
	if ( 0 < read( fd, r->name, sizeof(r->name) ) ) {
	  ndprintf( stdout, "[SERVER] robot didn't send its name...\n" );
        }

	return 1;
}

static volatile int timer;
static int winner;

void raise_timer(int sig)
{
    timer = 1;
}

int process_robots()
{
        int retVal = 0;
	int i, ret, rfd;
	struct pollfd *pfd;
	result_t result;
	char buf[STD_BUF];
	struct robot *robot;
	int to_talk;

	for (i = 0; i < max_robots; i++)
		fds[i].events = POLLIN | POLLOUT;

	do {
		to_talk = 0;
		rfd = -1;
		for (i = 0; i < max_robots; i++) {
			if (fds[i].fd != -1) {
				to_talk++;
				rfd = fds[i].fd;
			}
		}

		if (to_talk == 0) {
			if (winner)
				ndprintf(stdout, "[GAME] Winner found\n");
			else
				ndprintf(stdout, "[GAME] Ended - No winner\n");
                        sleep( 2 );
			// jagwas: exit(EXIT_SUCCESS);
                        retVal = 1; // tell upper code layers it's time to quit
		}
		else if (to_talk == 1)
			winner = 1;

		poll(fds, max_robots, 10);
		for (i = 0; i < max_robots; i++) {
			robot = all_robots[i];
			pfd = &fds[i];
			if (pfd->fd == -1) // Dead robot
				continue;

			if (pfd->events == 0)
				to_talk--;

			if (pfd->revents & (POLLERR | POLLHUP)) { /* Error or disconnected robot -> kill */
				close(pfd->fd);
				pfd->fd = -1;
				kill_robot(robot);
				continue;
			}
			if (robot->damage >= 100) {
				sockwrite(pfd->fd, DEAD, "Sorry!");
				close(pfd->fd);
				pfd->fd = -1;
				continue;
			}

			if (!(pfd->revents & POLLIN))
				continue;

			if (!(pfd->revents & POLLOUT)) {
				close(pfd->fd);
				pfd->fd = -1;
				kill_robot(robot);
				continue;
			}

			ret = read(pfd->fd, buf, STD_BUF);
			switch (ret) {
				case -1:
					close(pfd->fd);
					pfd->fd = -1;
					kill_robot(robot);
					break;
				case 0:
					close(pfd->fd);     // jag;
					pfd->fd = -1;       // jag;
					kill_robot(robot);  // jag;
					break;              // jag;
					// jagwas: abort ();
				default:
					buf[ret] = '\0';
					result = execute_cmd(robot, buf);
					if (result.error) {
						sockwrite(pfd->fd, ERROR, "Violation of the protocol!\n");
						close(pfd->fd);
						pfd->fd = -1;
						kill_robot(robot);
					}
					else {
						if (result.cycle)
							pfd->events = 0;
						if (winner && pfd->fd == rfd)
							sockwrite(pfd->fd, END, "%d", result.result);
						else
							sockwrite(pfd->fd, OK, "%d", result.result);
					}
					break;
			}
		}
	} while (to_talk && !timer);
        return retVal;
}

void
server_start (char *hostname, char *port)
{
	int sockd, ret, fd, i, opt = 1;
	struct addrinfo *ai, *runp, hints;
	struct sockaddr *addr;
	socklen_t addrlen = sizeof(addr);

	memset (&hints, 0x0, sizeof (hints));
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;

	ndprintf(stdout, "[SERVER] Starting Server at %s:%s\n[INFO] Number of players: %d\n", hostname, port, max_robots);

	if ((ret = getaddrinfo(hostname, port, &hints, &ai)))
		ndprintf_die(stderr, "[ERROR] getaddrinfo('%s', '%s'): %s\n", hostname, port, gai_strerror(ret));
	if (!ai)
		ndprintf_die(stderr, "[ERROR] getaddrinf(): couldn't fill the struct!\n");

	runp = ai;

	do {
		sockd = socket(runp->ai_family, runp->ai_socktype, runp->ai_protocol);
		if (sockd != -1)
			break;
		runp = runp->ai_next;
	} while (runp);
	if (sockd == -1)
		ndprintf_die(stderr, "[ERROR] socket(): Couldn't create socket!\n");

	/* To close the port after closing the socket */
	if (setsockopt(sockd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) == -1)
		ndprintf_die(stderr, "[ERROR] setsockopt(): %s\n", strerror(errno));
	if (bind(sockd, runp->ai_addr, runp->ai_addrlen))
		ndprintf_die(stderr, "[ERROR] bind(): %s\n", strerror(errno));
	if (listen(sockd, max_robots))
		ndprintf_die(stderr, "[ERROR] listen(): %s\n", strerror(errno));
	if (!(fds = (struct pollfd *) malloc (max_robots * sizeof(struct pollfd))))
		ndprintf_die(stderr, "[ERROR] Couldn't malloc space for fds!\n");
	
	while (1) { /* Wait for all the clients to connect */
		fd = accept(sockd, (struct sockaddr *) &addr, &addrlen);
		if (!create_client(fd))
			sockwrite(fd, ERROR, "Couldn't duplicate the FD\n");
		if (current_robots >= max_robots)
			break;
	}
	ndprintf(stdout, "[GAME] Starting. All clients connected!\n");
	for (i = 0; i < max_robots; i++)
		sockwrite(fds[i].fd, START, "Let's play!");

	signal (SIGALRM, raise_timer);
}

int server_cycle (SDL_Event *event)
{
	int i;
	if (current_cycles >= max_cycles) {
		for (i = 0; i < max_robots; i++) {
			if (fds[i].fd != -1) {
				sockwrite(fds[i].fd, DRAW, "Max cycles reached!\n");
				close(fds[i].fd);
			}
		}
		ndprintf(stdout, "[GAME] Ended - Draw!\n");
		exit(EXIT_SUCCESS);
	}
	current_cycles++;
	struct itimerval itv;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 10000;
	setitimer (ITIMER_REAL, &itv, NULL);
	timer = 0;
	cycle();
	update_display(event);
	return process_robots();
}

void usage(char *prog, int retval)
{
    printf("Usage %s [-n <clients> -H <hostname> -P <port> -d]\n"
           "\t-n <clients>\tNumber of clients to start the game (has to be bigger than 1) (Default: 5)\n"
           "\t-H <hostname>\tSpecifies hostname (Default: 127.0.0.1)\n"
           "\t-P <port>\tSpecifies port (Default: 4300)\n"
           "\t-d\tEnables debug mode\n", prog);
    exit(retval);
}

int server_init(int argc, char *argv[])
{
	char *port = STD_PORT;
        char *hostname = STD_HOSTNAME;

	int retval;
	while ((retval = getopt(argc, argv, "dn:hH:P:c:")) != -1) {
		switch (retval) {
			case 'c':
				max_cycles = atoi(optarg);
				break;	
			case 'H':
				hostname = optarg;
				break;
			case 'P':
				port = optarg;
				break;
			case 'd':
				debug = 1;
				break;
			case 'n':
				max_robots = atoi(optarg);
				break;
			case 'h':
				usage(argv[0], EXIT_SUCCESS);
				break;
			default:
				break;
		}
	}

	if (argc > optind) {
		usage(argv[0], EXIT_FAILURE);
        }

	if (max_robots <= 1) {
		max_robots = STD_CLIENTS;
        }

	if (max_cycles <= 1) {
		max_cycles = STD_CYCLES;
        }

	all_robots = (struct robot **)malloc(max_robots * sizeof(struct robot *));

	server_start(hostname, port);
	return 0;
}

