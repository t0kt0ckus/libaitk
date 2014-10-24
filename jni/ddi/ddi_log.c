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

#include "ddi_log.h"

static FILE *ddi_logfile_ptr = NULL;

static const char *DDI_LOG_FMT = "/data/local/tmp/aitk/logs/ddi-%05d.log";
static const int DDI_LOG_WLEN = 34 + 5;

static char DDI_LOG_BUFFER[255];


void ddi_log_fmt(char *fmt, ...) 
{
    if (ddi_logfile_ptr)
    {
        va_list argp;
        va_start(argp, fmt);
        vsnprintf(DDI_LOG_BUFFER, sizeof(DDI_LOG_BUFFER), fmt, argp);
        va_end(argp);

        fprintf(ddi_logfile_ptr, "%s", DDI_LOG_BUFFER);
        fflush(ddi_logfile_ptr);
    }
}

int ddi_log_init(int pid)
{
    int last_err = -1;
    int logpath_len = DDI_LOG_WLEN + 1;
    char *logpath;
    
    if ((logpath = malloc(sizeof(char) * logpath_len)))
    {
        snprintf(logpath, logpath_len, DDI_LOG_FMT, pid);
        if ((ddi_logfile_ptr = fopen(logpath, "w")))
        {
            chmod(logpath, 0644);
            last_err = 0;
        }
        else
            last_err = errno;
        free(logpath);
    }

    return last_err;
}
