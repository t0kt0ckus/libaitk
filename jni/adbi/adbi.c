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

#include "adbi.h"
#include "adbi_log.h"

void adbi_init()
{
    pid_t pid = getpid();
    adbi_log_init(pid);

    adbi_log_fmt("adbi_init(%d)\n", pid);
}



