/*
 * Aitk
 *
 * t0kt0ckus
 * (C) 2014
 *
 */
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "ddi.h"

void ddi_init()
{
    pid_t pid = getpid();
    ddi_logger = yal_log_init(DDI_LOG_DIR, DDI, pid);    

    time_t t0;
    time(&t0);
    ddi_log_printf("Starting DDI (PID: %d): %s\n", pid, ctime(&t0));
}


