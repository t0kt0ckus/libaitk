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
 * C) 2014
 *
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string.h>
#include <termios.h>
#include <pthread.h>
#include <sys/epoll.h>

#include "adbi.h"
#include "util.h"

void inline hook_cacheflush(unsigned int begin, unsigned int end)
{
    adbi_log_printf("->hook_cacheflush(0x%x, 0x%x)\n", begin, end);

	const int syscall = 0xf0002;
	__asm __volatile (
		"mov	 r0, %0\n"			
		"mov	 r1, %1\n"
		"mov	 r7, %2\n"
		"mov     r2, #0x0\n"
		"svc     0x00000000\n"
		:
		:	"r" (begin), "r" (end), "r" (syscall)
		:	"r0", "r1", "r7"
		);
    
    adbi_log_printf("<-hook_cacheflush()\n");
}

int hook_direct(struct hook_t *h, unsigned int addr, void *hookf)
{
	int i;
	adbi_log_printf("->hook_direct():\n"); 
	adbi_log_printf("addr  = %x\n", addr);
	adbi_log_printf("hookf = %x\n", hookf);

	if ((addr % 4 == 0 && (unsigned int)hookf % 4 != 0)
            || (addr % 4 != 0 && (unsigned int)hookf % 4 == 0))
		adbi_log_printf("addr 0x%x and hook 0x%x\n don't match!\n",
                addr, hookf);
	
	h->thumb = 0;
	h->patch = (unsigned int)hookf;
	h->orig = addr;
	adbi_log_printf("orig = %x\n", h->orig);
	h->jump[0] = 0xe59ff000; // LDR pc, [pc, #0]
	h->jump[1] = h->patch;
	h->jump[2] = h->patch;
	for (i = 0; i < 3; i++)
		h->store[i] = ((int*)h->orig)[i];
	for (i = 0; i < 3; i++)
		((int*)h->orig)[i] = h->jump[i];
	
	hook_cacheflush((unsigned int)h->orig, (unsigned int)h->orig+sizeof(h->jumpt));
	
    adbi_log_printf("<-hook_direct()\n"); 
	return 1;
}

int hook(struct hook_t *h, int pid, char *libname, char *funcname, void *hook_arm, void *hook_thumb)
{
	unsigned long int addr;
	int i;

	if (find_name(pid, funcname, libname, &addr) < 0) {
		adbi_log_printf("can't find: %s\n", funcname);
		return 0;
	}
	
	adbi_log_printf("hooking:   %s = 0x%x ", funcname, addr);
	strncpy(h->name, funcname, sizeof(h->name)-1);

	if (addr % 4 == 0) {
		adbi_log_printf("ARM using 0x%x\n", hook_arm);
		h->thumb = 0;
		h->patch = (unsigned int)hook_arm;
		h->orig = addr;
		h->jump[0] = 0xe59ff000; // LDR pc, [pc, #0]
		h->jump[1] = h->patch;
		h->jump[2] = h->patch;
		for (i = 0; i < 3; i++)
			h->store[i] = ((int*)h->orig)[i];
		for (i = 0; i < 3; i++)
			((int*)h->orig)[i] = h->jump[i];
	}
	else {
		if ((unsigned long int)hook_thumb % 4 == 0)
			adbi_log_printf("warning hook is not thumb 0x%x\n", hook_thumb);
		h->thumb = 1;
		adbi_log_printf("THUMB using 0x%x\n", hook_thumb);
		h->patch = (unsigned int)hook_thumb;
		h->orig = addr;	
		h->jumpt[1] = 0xb4;
		h->jumpt[0] = 0x30; // push {r4,r5}
		h->jumpt[3] = 0xa5;
		h->jumpt[2] = 0x03; // add r5, pc, #12
		h->jumpt[5] = 0x68;
		h->jumpt[4] = 0x2d; // ldr r5, [r5]
		h->jumpt[7] = 0xb0;
		h->jumpt[6] = 0x02; // add sp,sp,#8
		h->jumpt[9] = 0xb4;
		h->jumpt[8] = 0x20; // push {r5}
		h->jumpt[11] = 0xb0;
		h->jumpt[10] = 0x81; // sub sp,sp,#4
		h->jumpt[13] = 0xbd;
		h->jumpt[12] = 0x20; // pop {r5, pc}
		h->jumpt[15] = 0x46;
		h->jumpt[14] = 0xaf; // mov pc, r5 ; just to pad to 4 byte boundary
		memcpy(&h->jumpt[16], (unsigned char*)&h->patch, sizeof(unsigned int));
		unsigned int orig = addr - 1; // sub 1 to get real address
		for (i = 0; i < 20; i++) {
			h->storet[i] = ((unsigned char*)orig)[i];
		}
		for (i = 0; i < 20; i++) {
			((unsigned char*)orig)[i] = h->jumpt[i];
		}
	}
	hook_cacheflush((unsigned int)h->orig, (unsigned int)h->orig+sizeof(h->jumpt));
	return 1;
}

void hook_precall(struct hook_t *h)
{
	adbi_log_printf("->hook_precall(%s)\n", h->name);
	int i;
	
	if (h->thumb) {
		unsigned int orig = h->orig - 1;
		for (i = 0; i < 20; i++) {
			((unsigned char*)orig)[i] = h->storet[i];
		}
	}
	else {
		for (i = 0; i < 3; i++)
			((int*)h->orig)[i] = h->store[i];
	}	
	hook_cacheflush((unsigned int)h->orig, (unsigned int)h->orig+sizeof(h->jumpt));
    
	adbi_log_printf("<-hook_precall(%s)\n", h->name);
}

void hook_postcall(struct hook_t *h)
{
	int i;
	adbi_log_printf("->hook_postcall(%s)\n", h->name);
	
	if (h->thumb) {
		unsigned int orig = h->orig - 1;
		for (i = 0; i < 20; i++)
			((unsigned char*)orig)[i] = h->jumpt[i];
	}
	else {
		for (i = 0; i < 3; i++)
			((int*)h->orig)[i] = h->jump[i];
	}
	hook_cacheflush((unsigned int)h->orig, (unsigned int)h->orig+sizeof(h->jumpt));	

    adbi_log_printf("<-hook_postcall(%s)\n", h->name);
}

void unhook(struct hook_t *h)
{
	adbi_log_printf("unhooking %s = %x  hook = %x ",
            h->name,
            h->orig,
            h->patch);

	hook_precall(h);
}


