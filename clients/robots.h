#ifndef ROBOTS_H
#define ROBOTS_H

#include <stdio.h>

#define STD_RESP_LEN 64
#define DEFAULT_REMOTEHOST "localhost"
#define DEFAULT_PORT "4300"

int scan(int degree,int resolution);
int cannon(int degree,int range);
void drive(int degree,int speed);
int damage();
void cycle();
int speed();
int loc_x();
int loc_y();
void ndprintf_die(FILE *fd, char * fmt, ...);
void printf_die(FILE *fd, char * fmt, int err, ...);

#define main rmain

#endif
