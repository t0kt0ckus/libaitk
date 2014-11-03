/*
 *  Collin's Binary Instrumentation Tool/Framework for Android
 *  Collin Mulliner <collin[at]mulliner.org>
 *  http://www.mulliner.org/android/
 *
 *  (c) 2012,2013
 *
 *  License: LGPL v2.1
 *
 */
/*
 * Aitk
 *
 * t0kt0ckus
 * (C) 2014
 *
 */
#ifndef _ADBI_H
#define _ADBI_H

#include "../yal/yal.h"

// Required initialization
//
void adbi_init();

// Logging
//
yal_log_t *adbi_logger;

#define ADBI "adbi"
#define ADBI_LOG_DIR "/data/local/tmp/aitk/logs"

#define adbi_log_printf(...) yal_log_printf(adbi_logger, __VA_ARGS__)

// Hooking API
//
struct hook_t {
	unsigned int jump[3];
	unsigned int store[3];
	unsigned char jumpt[20];
	unsigned char storet[20];
	unsigned int orig;
	unsigned int patch;
	unsigned char thumb;
	unsigned char name[128];
	void *data;
};

void hook_cacheflush(unsigned int begin, unsigned int end);	
void hook_precall(struct hook_t *h);
void hook_postcall(struct hook_t *h);
int hook(struct hook_t *h, int pid, char *libname, char *funcname, void *hook_arm, void *hook_thumb);
void unhook(struct hook_t *h);

#endif
