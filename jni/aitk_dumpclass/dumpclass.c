/*
 * Aitk
 *
 * t0kt0ckus
 * C) 2014
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <pthread.h>

#include <jni.h>

#include "../adbi/adbi.h"
#include "../ddi/ddi.h"

// #define DUMP_CLASS_NAME "Lcom/android/internal/telephony/SMSDispatcher;"
#define DUMP_CLASS_NAME "Lcom/android/internal/telephony/gsm/GsmInboundSmsHandler;"


static struct hook_t eph;
static struct dexstuff_t d;
static int counter;

static int dvkdump_log_init();
static int aitk_dumpclass();
static void *aitk_dumpclass_fn(void * targs);

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
		adbi_log_printf("epoll_wait() called\n");
		counter--;
		
	    // resolve symbols from DVM
	    dexstuff_resolv_dvm(&d);
        
        // list all dvm-loaded classes
        pthread_t logcat_pth;
        pthread_attr_t attrs;
        pthread_attr_init(&attrs);
        pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
        pthread_create(&logcat_pth, &attrs, aitk_dumpclass_fn, DUMP_CLASS_NAME);  
	    
		if (!counter) 
			adbi_log_printf("removing hook for epoll_wait() on next event\n");
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

static const char *DVKDUMP_LOG_FMT = "/data/local/tmp/aitk/logs/dumpclass-%05d.log";
static const int DVKDUMP_LOG_WLEN = 40 + 5;

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
        if ((dvkdump_logfile_ptr = fopen(logpath, "a+")))
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

void *aitk_dumpclass_fn(void * targs)
{
    aitk_dumpclass((char *) targs);
    pthread_exit(NULL);
}

int aitk_dumpclass(char *clname)
{
    time_t t_start = time(NULL);
    ddi_log_printf("aitk_dumpclass(): ...");
    
    if ((! dvkdump_logfile_ptr) && (dvkdump_log_init())) {
        ddi_log_printf("Failed to init dump file, aborting !\n");
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
	    dalvik_dump_class(&d, clname);

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
    ddi_log_printf(" completed (%d s)\n", t_end - t_start);
    return 0;        
}

