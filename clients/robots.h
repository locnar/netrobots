#ifndef ROBOTS_H
#define ROBOTS_H 1

#include <stdio.h>

#define STD_RESP_LEN 64
#define DEFAULT_REMOTEHOST "localhost"
// #define DEFAULT_REMOTEHOST "192.168.1.63"
#define DEFAULT_PORT "4300"

int scan (int degree,int resolution);
int cannon (int degree,int range);
void drive (int degree,int speed);
int damage();
void cycle ();
int speed();
int loc_x();
int loc_y();
        /* jag; 12nov2018 -- begin  added code */
void ndprintf_die( FILE *fd, char * fmt, ...);
void printf_die(   FILE *fd, char * fmt, int err, ...);
        /* jag; 12nov2018 -- end of added code */

#define main rmain

#endif
