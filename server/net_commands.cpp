#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "robotserver.h"
#include "net_utils.h"
#include "net_defines.h"

extern int current_cycles;

int cmd_cycle(Robot * robot, int * args);
int cmd_cannon(Robot * robot, int * args);
int cmd_scan(Robot * robot, int * args);
int cmd_loc_x(Robot * robot, int * args);
int cmd_loc_y(Robot * robot, int * args);
int cmd_damage(Robot * robot, int * args);
int cmd_speed(Robot * robot, int * args);
int cmd_drive(Robot * robot, int * args);
int cmd_time(Robot * robot, int * args);

cmd_t cmds[] = {
    { cmd_cycle,   0, true  },   // CYCLE
    { cmd_cannon,  2, true  },   // CANNON
    { cmd_scan,    2, true  },   // SCAN
    { cmd_loc_x,   0, false },   // LOC_X
    { cmd_loc_y,   0, false },   // LOC_Y
    { cmd_damage,  0, false },   // DAMAGE
    { cmd_speed,   0, false },   // SPEED
    { cmd_drive,   2, true  },   // DRIVE
    { cmd_time,    0, false },   // TIME
};

result_t error_res = { -1, true, false };

int const NUMBER_COMMANDS = sizeof(cmds)/sizeof(cmd_t);

int cmd_cycle(Robot * /*robot*/, int * /*args*/)
{
    return 1;
}

int cmd_scan(Robot * robot, int * args)
{
    return scan(robot, args[0], args[1]);
}

int cmd_cannon(Robot * robot, int * args)
{
    return cannon(robot, args[0], args[1]);
}

int cmd_loc_x(Robot * robot, int * /*args*/)
{
    return loc_x(robot);
}

int cmd_loc_y(Robot * robot, int * /*args*/)
{
    return loc_y(robot);
}

int cmd_damage(Robot * robot, int * /*args*/)
{
    return damage(robot);
}

int cmd_speed(Robot * robot, int * /*args*/)
{
    return speed(robot);
}

int cmd_drive(Robot * robot, int * args)
{
    drive(robot, args[0], args[1]);
    return 1;
}

int cmd_time(Robot * /*robot*/, int * /*args*/)
{
    return current_cycles;
}

result_t execute_cmd(Robot * robot, char *input)
{
    char **argv;
    int const argc = str_to_argv(input, &argv);
    if (argc == -1) {
        return error_res;
    }
    if (!argc || !str_isnumber(argv[0])) {
        free(argv);
        return error_res;
    }

    int const i = atoi(argv[0]);
    if (i < 0 || i >= NUMBER_COMMANDS) {
        free(argv);
        return error_res;
    }
    cmd_t const & cmd = cmds[i];

    if (cmd.args != argc - 1) {
        free(argv);
        return error_res;
    }

    int * args = (int *)malloc(cmd.args * sizeof(int));
    if (!args) {
        free(argv);
        return error_res;
    }

    for (int idx = 1; idx < argc; idx++) {
        if (!str_isnumber(argv[idx])) {
            free(argv);
            free(args);
            return error_res;
        }
        args[idx - 1] = atoi(argv[idx]);
    }


    int const ret = cmd.func(robot, args);
    ndprintf(stdout, "[COMMAND] %s -> %d received - %g %g %d\n", argv[0], ret, robot->x, robot->y, robot->damage);
    if (ret == -1) {
        free(argv);
        free(args);
        return error_res;
    }

    result_t res;
    res.result = ret;
    res.cycle = cmd.cycle;
    res.error = false;

    free(argv);
    free(args);
    return res;
}
