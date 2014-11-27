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

#define SEND_RAW_PDU_DEXFILE "/data/local/tmp/aitk/libs/libaitk_classes.dex"
#define SEND_RAW_PDU_CLASS "org/openmarl/libaitk/SendRawPdu"

static JavaVM *aitk_resolve_local_jvm();
static JNIEnv *aitk_resolve_local_jenv(JavaVM *gVm); 

static struct hook_t eph;
static struct dexstuff_t dexEnv;
static int hook_counter;

static int aitk_send_raw_pdu();
static void *aitk_send_raw_pdu_fn(void * targs);

extern int my_epoll_wait_arm(int epfd,
        struct epoll_event *events,
        int maxevents, 
        int timeout);

int my_epoll_wait(int epfd,
     struct epoll_event *events,
      int maxevents, 
      int timeout)
{
    adbi_log_printf("epoll_wait() called\n");
	
    int (*orig_epoll_wait)
	    (int epfd, struct epoll_event *events, int maxevents, int timeout);
	    
	orig_epoll_wait = (void*)eph.orig;
	hook_precall(&eph); // remove hook (see github)

	int res = orig_epoll_wait(epfd, events, maxevents, timeout);

	if ( hook_counter-- )
    {
        adbi_log_printf("starting hooked code execution ...");

		hook_postcall(&eph);		
	    dexstuff_resolv_dvm(&dexEnv);

        JavaVM *gVm = aitk_resolve_local_jvm();
        if (gVm)
        {
            JNIEnv *jniEnv = aitk_resolve_local_jenv(gVm);
            if (jniEnv)
            {
                int cookie = dexstuff_loaddex(&dexEnv, SEND_RAW_PDU_DEXFILE);
                ddi_log_printf("send_raw_pdu.dex: %x\n", cookie);

                //FIXME: cookie is 0, but seems to work so far
                void *dex_SendRawPdu = dexstuff_defineclass(&dexEnv,
                        SEND_RAW_PDU_CLASS, 
                        cookie);
                ddi_log_printf("SendRawPdu (dex): %x\n", dex_SendRawPdu);

                if (dex_SendRawPdu)
                {
                    jclass jcl_SendRawPdu = (*jniEnv)->FindClass(jniEnv, SEND_RAW_PDU_CLASS);
                    ddi_log_printf("SendRawPdu (Java): %x\n", jcl_SendRawPdu);

                    if (jcl_SendRawPdu)
                    {
                        jmethodID jinit_SendRawPdu = (*jniEnv)->GetMethodID(jniEnv,
                                jcl_SendRawPdu,
                                "<init>",
                                "()V");
                        ddi_log_printf("SendRawPdu (<init>): %x\n", jinit_SendRawPdu);

                        if (jinit_SendRawPdu)
                        {
                            jobject jobj_SendRawPdu = (*jniEnv)->NewObject(jniEnv, 
                                    jcl_SendRawPdu, 
                                    jinit_SendRawPdu);
                            ddi_log_printf("SendRawPdu instance: %x\n", jobj_SendRawPdu);
                        }
                    }

                }
            }
        }

        adbi_log_printf("removing hook for epoll_wait() on next event\n");
	}
	
	return res;
}

void __attribute__ ((constructor)) init_so_send_raw_pdu(void);
void init_so_send_raw_pdu(void)
{
    adbi_init();
    ddi_init();
    hook_counter = 1;  

    hook(&eph, getpid(), "libc.", "epoll_wait", 
        my_epoll_wait_arm,
        my_epoll_wait);
}

void *aitk_send_raw_pdu_fn(void * targs)
{
    aitk_send_raw_pdu((char *) targs);
    pthread_exit(NULL);
}

int aitk_send_raw_pdu(char *clname)
{
    time_t t_start = time(NULL);
    ddi_log_printf("aitk_send_raw_pdu(): ...");
    

    time_t t_end = time(NULL);
    ddi_log_printf(" completed (%d s)\n", t_end - t_start);
    return 0;        
}

JavaVM *aitk_resolve_local_jvm()
{
    JavaVM *gVm = NULL;

    int n;
    JavaVM* jvms[1];

    if ((JNI_GetCreatedJavaVMs(jvms, 1, &n) != JNI_OK) || (n == 0))
    {
        ddi_log_printf("Failed to resolve any Java VM !\n");
    }
    else 
    {
        gVm = jvms[0];
        ddi_log_printf("Local VM: %x\n", gVm); 
    }

    return gVm;
}

JNIEnv *aitk_resolve_local_jenv(JavaVM *gVm)
{
    JNIEnv *env;
    
    jint res = (*gVm)->GetEnv(gVm, (void **) &env, JNI_VERSION_1_6);
    if (res == JNI_EDETACHED)
    {
        ddi_log_printf("JNI_EDETACHED: tryinng to attach ...\n");
        (*gVm)->AttachCurrentThread(gVm, &env, NULL);
    }
    else if (res == JNI_EVERSION)
    {
        ddi_log_printf("JNI_EVERSION !\n");
    }

    if (env == NULL)
        ddi_log_printf("Failed to resolve local JNI environment !\n");

    return env;
}
