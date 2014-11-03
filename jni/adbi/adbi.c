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
#include <time.h>

#include "adbi.h"

void adbi_init()
{
    pid_t pid = getpid();
    adbi_logger = yal_log_init(ADBI_LOG_DIR, ADBI, pid);    

    time_t t0;
    time(&t0);
    adbi_log_printf("Starting ADBI (PID: %d): %s\n", pid, ctime(&t0));
}



