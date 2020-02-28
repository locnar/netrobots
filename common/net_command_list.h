#ifndef NET_COMMAND_H
#define NET_COMMAND_H

namespace CommandEnum {
    enum Type {
        Cycle    = 0,
        Cannon   = 1,
        Scan     = 2,
        LocX     = 3,
        LocY     = 4,
        Damage   = 5,
        Speed    = 6,
        Drive    = 7,
        PROGNAME = 8,
    };
}

namespace ResponseEnum {
    enum Type {
        Error = 0,
        Ok    = 1,
        Start = 2,
        End   = 3,
        Dead  = 4,
        Draw  = 5,
    };
}

#endif

