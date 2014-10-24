/*
 * Aitk
 *
 * t0kt0ckus
 * (C) 2014
 *
 * License GPLv3
 *
 */
 
 // Requirements: directory /data/local/tmp/aitk/logs must exist and be world-writable
 
#ifndef _ADBI_LOG_H
#define _ADBI_LOG_H

#include <stdarg.h>

int adbi_log_init(int pid);

void adbi_log_fmt(char *fmt, ...);

#endif
