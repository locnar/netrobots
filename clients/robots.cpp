#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <unistd.h>

#include "robots.h"
#include "net_utils.h"
#include "net_command_list.h"

#undef main

extern int rmain();

static int serverfd;

static int eval_response(int resp)
{
    switch (resp) {
        case ResponseEnum::End:
            sockwrite(serverfd, ResponseEnum::End, nullptr);
            printf_die(stdout, 0, "You win!\n");
            break;

        case ResponseEnum::Dead:
            printf_die(stdout, 1, "You are dead!\n");
            break;

        case ResponseEnum::Start:
        case ResponseEnum::Ok:
            break;

        case ResponseEnum::Draw:
            printf_die(stdout, 0, "Game ended wih a draw!\n");
            break;

        case ResponseEnum::Error:
        default:
            printf_die(stderr, 2, "Error detected.. exiting!\n");
            break;
    }
    return resp;
}

static int get_resp_value(int ret)
{
    int result = -1;

    char resp[STD_RESP_LEN + 1];
    int const count = read(serverfd, resp, STD_RESP_LEN);
    if (count > 0) {
        resp[count] = '\0';

        char **argv;
        int const argc = str_to_argv(resp, &argv);
        if (argc == -1) {
            printf_die(stderr, EXIT_FAILURE, "[ERROR] Cannot allocate buffer... exiting!\n");
        }

        if (argc >= 1 && str_isnumber(argv[0])) {
            eval_response(atoi(argv[0]));
            if (argc >= 2 && str_isnumber(argv[1])) {
                result = atoi(argv[1]);
            }
            else {
                result = 0;
            }
        }
        free(argv);
    }

    if (ret == -1) {
        printf_die(stdout, errno, "Server probably dead or you have been killed!\n");
    }

    return result;
}

static int client_init(char * remotehost, char * port, char * progname)
{
    int result = 1;

    struct addrinfo hints;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_flags = AI_ADDRCONFIG;
    hints.ai_socktype = SOCK_STREAM;

    int ret;
    struct addrinfo * ai = nullptr;
    if ((ret = getaddrinfo (remotehost, port, &hints, &ai))) {
        printf_die(stderr, EXIT_FAILURE, "[ERROR] getaddrinfo('%s', '%s'): %s\n", remotehost, port, gai_strerror(ret));
    }
    if (!ai) {
        printf_die(stderr, EXIT_FAILURE, "[ERROR] getaddrinf(): couldn't fill the struct!\n");
    }

    struct addrinfo *runp = ai;
    while (runp) {
        int const sock = socket(runp->ai_family, runp->ai_socktype, runp->ai_protocol);
        if (sock != -1) {
            ndprintf(stdout, "[NETWORK] Connecting to server...\n");
            if(connect (sock, runp->ai_addr, runp->ai_addrlen) == 0) {
                ndprintf(stdout, "[NETWORK] connected to server\n");
                serverfd = sock;
                ret = write( serverfd, progname, STD_BUF );
                result = get_resp_value(0);
                break;
            }
            close(sock);
        }
        runp = runp->ai_next;
    }
    freeaddrinfo(ai);
    return result;
}


void usage(char *prog, int retval)
{
    printf("Usage %s [-n <clients> -H <hostname> -P <port> -d]\n"
           "\t-H <remothost>\tSpecifies hostname (Default: 127.0.0.1)\n"
           "\t-P <port>\tSpecifies port (Default: %s)\n"
           "\t-d\tEnables debug mode\n", prog, DEFAULT_PORT);
    exit(retval);
}

int main(int argc, char **argv)
{
    char * remotehost = DEFAULT_REMOTEHOST;
    char * port = DEFAULT_PORT;
    char progname[STD_BUF];   // must match size of name element of struct robot in server/robotserver.h
	
    strncpy(progname, basename(argv[0]), sizeof(progname));
    progname[sizeof(progname)-1] = '\0';  // make sure strings ends with NUL

    int retval;
    while ((retval = getopt(argc, argv, "dn:hH:P:")) != -1) {
        switch (retval) {
            case 'H':
                remotehost = optarg;
                break;

            case 'P':
                port = optarg;
                break;

            case 'd':
                debug = 1;
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

    signal(SIGPIPE, SIG_IGN);
    if (client_init(remotehost, port, progname)) {
        printf_die(stderr, EXIT_FAILURE, "could not connect to : %s:%s\n", remotehost, port);
    }

    srandom(time(nullptr) + getpid());
    srand(time(nullptr) + getpid());
    rmain ();
}

int scan(int degree,int resolution)
{
    int const ret = sockwrite(serverfd, CommandEnum::Scan, "%d %d", degree, resolution);
    return get_resp_value(ret);	
}

int cannon(int degree,int range)
{
    int const ret = sockwrite(serverfd, CommandEnum::Cannon, "%d %d", degree, range);
    return get_resp_value(ret);
}

void drive(int degree,int speed)
{
    int const ret = sockwrite(serverfd, CommandEnum::Drive,  "%d %d", degree, speed);
    get_resp_value(ret);
}

int damage()
{
    int const ret = sockwrite(serverfd, CommandEnum::Damage, nullptr);
    return get_resp_value(ret);
}

void cycle()
{
    int const ret = sockwrite(serverfd, CommandEnum::Cycle, nullptr);
    get_resp_value(ret);
}

int speed()
{
    int const ret = sockwrite(serverfd, CommandEnum::Speed, nullptr);
    return get_resp_value(ret);
}

int loc_x()
{
    int const ret = sockwrite(serverfd, CommandEnum::LocX, nullptr);
    return get_resp_value(ret);
}

int loc_y()
{
    int const ret = sockwrite(serverfd, CommandEnum::LocY, nullptr);
    return get_resp_value(ret);
}
