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
 
#ifndef _DDI_LOG_H
#define _DDI_LOG_H

#include <stdarg.h>

int ddi_log_init(int pid);

void ddi_log_fmt(char *fmt, ...);

#endif
