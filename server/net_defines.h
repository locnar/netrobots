#ifndef NET_DEFINES_H
#define NET_DEFINES_H

#include "robotserver.h"
#include "net_command_list.h"

typedef int (*HANDLER)(Robot *robot, int *args);

typedef struct cmd_t {
    HANDLER func;
    int args;
    bool cycle;
} cmd_t;

typedef struct result_t {
    int result;
    bool error;
    bool cycle;
} result_t;

void init_server(char *hostname, char *port);
result_t execute_cmd(Robot *robot, char *input);

#endif
