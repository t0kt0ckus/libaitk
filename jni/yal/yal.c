/*
 * Aitk
 *
 * t0kt0ckus
 * (C) 2014
 * 
 * License GPLv3
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "yal.h"

static void delete_logger(yal_log_t *logger);

yal_log_t *yal_log_init(const char *logdir_path,
        const char *appname,
        int pid) 
{
    int logfile_path_sz = sizeof(char) *
                (strlen(logdir_path) + 1 + strlen(appname) + 1 + 5 + 4 + 1); 

    yal_log_t *logger = malloc(sizeof(yal_log_t));
    if (logger)
    {
        logger->filepath = malloc(logfile_path_sz);
        if (logger->filepath)
            sprintf(logger->filepath,
                    "%s/%s-%05d.log",
                    logdir_path,
                    appname,
                    pid);

        if (logger->filepath) 
            logger->file = fopen(logger->filepath, "w");

        if (logger->filepath && logger->file)
            return logger;
        else
            delete_logger(logger);
    }
   
    printf("yal_log: Error: %s\n", strerror(errno));
    return NULL;
}

void yal_log_printf(yal_log_t *logger, char *fmt, ...)
{
  if (logger && logger->file)
  {
      va_list vargs;
      va_start(vargs, fmt);
      vfprintf(logger->file, fmt, vargs);
      va_end(vargs);

      fflush(logger->file);
  }
}

void yal_log_perror(yal_log_t *logger, int errnum)
{
    if (logger && logger->file)
    {
        fprintf(logger->file, "[Error: %d] %s\n", errnum, strerror(errnum));
        fflush(logger->file);
    }
}

void yal_log_printerrf(yal_log_t *logger, int errnum, char *fmt, ...)
{
    if (logger && logger->file)
    {
        fprintf(logger->file, "[Error: %d] ", errnum);

        va_list vargs;
        va_start(vargs, fmt);
        vfprintf(logger->file, fmt, vargs); 
        va_end(vargs);

        fflush(logger->file);
    }
}

void yal_log_close(yal_log_t *logger)
{
    if (logger && logger->file)
        fclose(logger->file);
    delete_logger(logger);
}

void delete_logger(yal_log_t *logger)
{
    if (logger)
    {
        if (logger->filepath) free(logger->filepath);
        if (logger->file) free(logger->file);
        free(logger);
    }
}

