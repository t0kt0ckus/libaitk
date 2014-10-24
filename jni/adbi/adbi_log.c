/*
 * Aitk
 *
 * t0kt0ckus
 * (C) 2014
 *
 * License GPLv3
 *
 */
#include <unistd.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "adbi_log.h"

static FILE *adbi_logfile_ptr = NULL;

static const char *ADBI_LOG_FMT = "/data/local/tmp/aitk/logs/adbi-%05d.log";
static const int ADBI_LOG_WLEN = 35 + 5;

static char ADBI_LOG_BUFFER[255];


void adbi_log_fmt(char *fmt, ...) 
{
    if (adbi_logfile_ptr)
    {
        va_list argp;
        va_start(argp, fmt);
        vsnprintf(ADBI_LOG_BUFFER, sizeof(ADBI_LOG_BUFFER), fmt, argp);
        va_end(argp);

        fprintf(adbi_logfile_ptr, "%s", ADBI_LOG_BUFFER);
        fflush(adbi_logfile_ptr);
    }
}

int adbi_log_init(int pid)
{
    int last_err = -1;
    int logpath_len = ADBI_LOG_WLEN + 1;
    char *logpath;
    
    if ((logpath = malloc(sizeof(char) * logpath_len)))
    {
        snprintf(logpath, logpath_len, ADBI_LOG_FMT, pid);
        if ((adbi_logfile_ptr = fopen(logpath, "w")))
        {
            chmod(logpath, 0666);
            last_err = 0;
        }
        else
            last_err = errno;
        free(logpath);
    }

    return last_err;
}
