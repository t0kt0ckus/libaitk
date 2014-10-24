/*
 * Aitk
 *
 * t0kt0ckus
 * (C) 2014
 *
 * License GPLv3
 *
 */
#ifndef _ADBI_H
#define _ADBI_H

void adbi_init();

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

int start_coms(int *coms, char *ptsn);

void hook_cacheflush(unsigned int begin, unsigned int end);	
void hook_precall(struct hook_t *h);
void hook_postcall(struct hook_t *h);
int hook(struct hook_t *h, int pid, char *libname, char *funcname, void *hook_arm, void *hook_thumb);
void unhook(struct hook_t *h);

#endif
