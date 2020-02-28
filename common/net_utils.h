#ifndef SERVERUTILS_H
#define SERVERUTILS_H

#include <stdio.h>

#define MAX_ARGC 16
#define STD_ALLOC 4
#define STD_BUF 64

extern bool debug;

/* to pass the ***argv pass &argv where argv is an array of strings */
int str_to_argv(char *str, char ***argv);
char* argv_to_str(char **argv);
void ndprintf(FILE *fd, char *fmt, ...) __attribute__((format(printf, 2, 3)));
void ndprintf_die(FILE *fd, char *fmt, ...) __attribute__((format(printf, 2, 3)));
int sockwrite(int fd, int status, char *fmt, ...) __attribute__((format(printf, 3, 4)));
bool str_isnumber(char const * str);

#endif

