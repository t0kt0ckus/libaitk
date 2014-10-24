/*
 *  Collin's Dynamic Dalvik Instrumentation Toolkit for Android
 *  Collin Mulliner <collin[at]mulliner.org>
 *
 *  (c) 2012,2013
 *
 *  License: LGPL v2.1
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <string.h>
#include <sys/epoll.h>

#include <jni.h>

#include "../adbi/adbi.h"

#include "../ddi/dexstuff.h"
#include "../ddi/dalvik_hook.h"
#include "../ddi/ddi_log.h"
#include "../ddi/ddi.h"

static struct hook_t eph;
static struct dexstuff_t d;
static int counter;

static int dvkdump_log_init();
static int aitk_dumpall();

// helper function
void printString(JNIEnv *env, jobject str, char *l)
{
	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("%s%s\n", l, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}
}

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
		
	    // resolve symbols from DVM
	    dexstuff_resolv_dvm(&d);
        
        // list all dvm-loaded classes
        aitk_dumpall();
	    
		if (!counter) 
			adbi_log_fmt("removing hook for epoll_wait() on next event\n");
	}
	
	return res;
}

void __attribute__ ((constructor)) init_so_ddi_test(void);
void init_so_ddi_test(void)
{
    adbi_init();
    ddi_init();
    counter = 1;   
    
    hook(&eph, getpid(), "libc.", "epoll_wait", 
        my_epoll_wait_arm,
        my_epoll_wait);
}

static FILE *dvkdump_logfile_ptr = NULL;

static const char *DVKDUMP_LOG_FMT = "/data/local/tmp/aitk/logs/dumpall-%05d.log";
static const int DVKDUMP_LOG_WLEN = 38 + 5;

static char * const LOGCAT_SH_ENV[] = {
            "PATH=/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin", NULL
};

int dvkdump_log_init()
{
    int last_err = -1;
    int logpath_len = DVKDUMP_LOG_WLEN + 1;
    char *logpath;
    
    if ((logpath = malloc(sizeof(char) * logpath_len)))
    {
        snprintf(logpath, logpath_len, DVKDUMP_LOG_FMT, getpid());
        if ((dvkdump_logfile_ptr = fopen(logpath, "w")))
        {
            chmod(logpath, 0644);
            last_err = 0;
        }
        else
            last_err = errno;
        free(logpath);
    }

    return last_err;
}

int aitk_dumpclass()
{
    time_t t_start = time(NULL);
    ddi_log_fmt("aitk_dumpclass():\n");
    
    if ((! dvkdump_logfile_ptr) && (dvkdump_log_init())) {
        ddi_log_fmt("Failed to init dump file, aborting !\n");
        return -1;
    }
    
    int dump_fd = fileno(dvkdump_logfile_ptr);

    pid_t logcat_pid = fork();
    if (logcat_pid < 0)
        return errno;

    if (logcat_pid == 0) {
        char *logcat_binary = "/system/bin/logcat";
        char *const params[4] = {logcat_binary, "dalvikvm:I", "*:S", NULL};

        dup2(dump_fd, 1);
        dup2(1,2);

        execve(logcat_binary, params, LOGCAT_SH_ENV);
    }
    else {
	    dalvik_dump_class(&d, "");

        time_t last_write, tnow;
        struct stat dump_stat;

        fstat(dump_fd, &dump_stat);
        last_write = dump_stat.st_mtime;

        while ( ((tnow = time(NULL)) - last_write) < 1 )
        {
            fstat(dump_fd, &dump_stat);
            last_write = dump_stat.st_mtime;
            sleep(1);
        }
        
        kill(logcat_pid, SIGTERM);
        int wstatus;
        waitpid(logcat_pid, &wstatus, 0);        
        close(dump_fd);
    }

    time_t t_end = time(NULL);
    ddi_log_fmt("aitk_dumpall() completed (%d s)\n", t_end - t_start);
    return 0;        
}

