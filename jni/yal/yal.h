/*
 * Aitk
 *
 * t0kt0ckus
 * (C) 2014
 * 
 * License GPLv3
 * 
 */
#include <stdarg.h>
#include <stdio.h>

#ifndef _YAL_H
#define _YAL_H

typedef struct yal_log {

    char *filepath;
    FILE *file;

} yal_log_t;

// initializes log file at: <logdir>/<appname>-<%05dpid>.log"
//
yal_log_t *yal_log_init(const char *logdir_path,
        const char *appname,
        int pid);

void yal_log_close(yal_log_t *logger);

void yal_log_printf(yal_log_t *logger, char *fmt, ...);

void yal_log_perror(yal_log_t *logger, int errnum);

void yal_log_printerrf(yal_log_t *logger, int errnum, char *fmt, ...);

void yal_log_close(yal_log_t *logger);

#endif
