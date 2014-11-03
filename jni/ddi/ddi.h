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
#include "../yal/yal.h"
#include "dexstuff.h"

#ifndef _DDI_H
#define _DDI_H

// Required initialization
//
void ddi_init();

// Logging
//

yal_log_t *ddi_logger;

#define DDI "ddi"
#define DDI_LOG_DIR "/data/local/tmp/aitk/logs"

#define ddi_log_printf(...) yal_log_printf(ddi_logger, __VA_ARGS__)

// Hooking API
//

struct dalvik_hook_t
{
	char clname[256];
	char clnamep[256];
	char method_name[256];
	char method_sig[256];

	Method *method;
	int sm; // static method

	// original values, saved before patching
	int iss;
	int rss;
	int oss;	
	int access_flags;
	void *insns; // dalvik code

	// native values
	int n_iss; // == n_rss
	int n_rss; // num argument (+ 1, if non-static method) 
	int n_oss; // 0
	void *native_func;
	
	int af; // access flags modifier
	
	int resolvm;

	// for the call
	jclass cls;
	jmethodID mid;

	// debug stuff
	int dump;      // call dvmDumpClass() while patching
	int debug_me;  // print debug while operating on this method
};

void* dalvik_hook(struct dexstuff_t *dex, struct dalvik_hook_t *h);
int dalvik_prepare(struct dexstuff_t *dex, struct dalvik_hook_t *h, JNIEnv *env);
void dalvik_postcall(struct dexstuff_t *dex, struct dalvik_hook_t *h);
int dalvik_hook_setup(struct dalvik_hook_t *h, char *cls, char *meth, char *sig, int ns, void *func);
void dalvik_dump_class(struct dexstuff_t *dex, char *clname);

#endif
