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
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>

#include <stdlib.h>

#include "../adbi/adbi.h"
#include "../adbi/adbi_log.h"


static struct hook_t eph;
static int counter;

// arm version of hook
extern int my_epoll_wait_arm(int epfd,
        struct epoll_event *events,
        int maxevents, 
        int timeout);
 
int my_epoll_wait(int epfd,
        struct epoll_event *events,
        int maxevents,
        int timeout)
{
	int (*orig_epoll_wait)
        (int epfd, struct epoll_event *events, int maxevents, int timeout);
        
	orig_epoll_wait = (void*)eph.orig;
	// remove hook for epoll_wait
	hook_precall(&eph);
	
	int res = orig_epoll_wait(epfd, events, maxevents, timeout);
	if (counter) {
		hook_postcall(&eph);
		adbi_log_fmt("epoll_wait() called\n");
		counter--;
		
		if (!counter) 
			adbi_log_fmt("removing hook for epoll_wait() on next event\n");
	}
        
	return res;
}

void __attribute__ ((constructor)) init_so_adbi_test(void);
void init_so_adbi_test(void)
{
    adbi_init();
	counter = 1;
    
	hook(&eph, getpid(),
            "libc.",
            "epoll_wait",
            my_epoll_wait_arm,
            my_epoll_wait);
}

