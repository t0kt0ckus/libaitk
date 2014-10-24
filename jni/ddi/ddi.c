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

#include "ddi.h"
#include "ddi_log.h"

void ddi_init()
{
    pid_t pid = getpid();
    ddi_log_init(pid);

    ddi_log_fmt("ddi_init(%d)\n", pid);
}



