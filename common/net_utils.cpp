#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <unistd.h>
#include "net_utils.h"

int debug = 0;

int str_to_argv(char *str, char ***argv)
{
    int argc = 0, alloc = STD_ALLOC;
    char **targv;

    *argv = NULL;

    if (!*str) {
        return argc;
    }

    if (!(targv = static_cast<char **>(malloc((1 + alloc) * sizeof(char *))))) {
        return -1;
    }

    for (;;) {
        while (*str && isspace(*str)) {
            *str++ = '\0';
        }

        if (!*str) {
            break;
        }

        if (argc >= MAX_ARGC) {
            break;
        }

        if (argc >= alloc) {
            alloc *= 2;
            if (!(targv = static_cast<char **>(realloc(targv, (1 + alloc) * sizeof(char *))))) {
                return argc;
            }
        }
        targv[argc++] = str;
        while (*str && !isspace(*str)) {
            str++;
        }
        if (!*str) {
            break;
        }
    }
    targv[argc] = nullptr;
    *argv = targv;

    return argc;
}

char* argv_to_str(char **argv)
{
    int alloc = STD_BUF, len = 0;

    char * buf = static_cast<char *>(malloc(alloc * sizeof(char)));
    if (!buf) {
        return nullptr;
    }

    for (int i = 0; argv[i]; i++) {
        char * token = argv[i];
        int slen = strlen(token);
        if (slen >= alloc + len + 1) {
            alloc = MAX(2 * len, 2 * slen + len + 1);

            buf = static_cast<char *>(realloc(buf, alloc * sizeof(char)));
            if (!buf) {
                return nullptr;
            }
        }
        memcpy(buf+len, token, slen);
        len += slen;
        buf[len++] = ' ';
    }
    buf[len] = '\0';

    return buf;
}


void ndprintf(FILE *fd, char *fmt, ...)
{
    if (debug) {
        va_list vp;

        va_start (vp, fmt);
        vfprintf(fd, fmt, vp);
        va_end(vp);
    }
}

void ndprintf_die(FILE *fd, char *fmt, ...)
{
    if (debug) {
        va_list vp;

        va_start(vp, fmt);
        vfprintf(fd, fmt, vp);
        va_end(vp);
    }
    exit(EXIT_FAILURE);
}

void printf_die(FILE *fd, char *fmt, int err, ...)
{
    va_list vp;

    va_start(vp, err);
    vfprintf(fd, fmt, vp);
    va_end(vp);

    exit(err);
}

int sockwrite(int fd, int status, char *fmt, ...)
{
    char *str, *tmp;

    int ret;
    if (fmt) {
        va_list vp;

        va_start(vp, fmt);
        ret = vasprintf(&tmp, fmt, vp);
        va_end(vp);
        ret = asprintf(&str, "%d %s", status, tmp);
    }
    else {
        ret = asprintf(&str, "%d", status);
    }

    ret = write(fd, str, strlen(str));

    if (fmt) {
        free(tmp);
    }

    free(str);
    return ret;
}

int str_isnumber(char *str)
{
    int const len = strlen(str);

    for (int i = 0; i < len; i++) {
        if (i == 0 && str[i] == '-') {
            continue;
        }

        if (!isdigit(str[i])) {
            return 0;
        }
    }

    return 1;
}
