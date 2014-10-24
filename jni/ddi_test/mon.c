/*
 *  Collin's Dynamic Dalvik Instrumentation Toolkit for Android
 *  Collin Mulliner <collin[at]mulliner.org>
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

static struct dalvik_hook_t sb1;
static struct dalvik_hook_t sb2;
static struct dalvik_hook_t sb3;
static struct dalvik_hook_t sb5;
static struct dalvik_hook_t sb6;
static struct dalvik_hook_t sb7;
static struct dalvik_hook_t sb8;
static struct dalvik_hook_t sb9;
static struct dalvik_hook_t sb10;
static struct dalvik_hook_t sb11;
static struct dalvik_hook_t sb13;
static struct dalvik_hook_t sb14;
static struct dalvik_hook_t sb20;

// helper function
void printString(JNIEnv *env, jobject str, char *l)
{
	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("%s%s\n", l, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}
}

// patches
static void* sb1_tostring(JNIEnv *env, jobject obj)
{
	dalvik_prepare(&d, &sb1, env);
	void *res = (*env)->CallObjectMethod(env, obj, sb1.mid); 
	//ddi_log_fmt("success calling : %s\n", sb1.method_name)
	dalvik_postcall(&d, &sb1);

	char *s = (*env)->GetStringUTFChars(env, res, 0);
	if (s) {
		ddi_log_fmt("sb1.toString() = %s\n", s);
		(*env)->ReleaseStringUTFChars(env, res, s); 
	}

	return res;
}

static void* sb20_tostring(JNIEnv *env, jobject obj)
{
/*
	ddi_log_fmt("tostring\n")
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
*/

	dalvik_prepare(&d, &sb20, env);
	void *res = (*env)->CallObjectMethod(env, obj, sb20.mid); 
	//ddi_log_fmt("success calling : %s\n", sb20.method_name)
	dalvik_postcall(&d, &sb20);

	char *s = (*env)->GetStringUTFChars(env, res, 0);
	if (s) {
		ddi_log_fmt("sb20.toString() = %s\n", s);
		(*env)->ReleaseStringUTFChars(env, res, s); 
	}

	return res;
}

static void* sb2_compareto(JNIEnv *env, jobject obj, jobject str)
{
/*
	ddi_log_fmt("compareto\n")
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[1];
	args[0].l = str;
	dalvik_prepare(&d, &sb2, env);
	int res = (*env)->CallIntMethodA(env, obj, sb2.mid, args); 
	//ddi_log_fmt("success calling : %s\n", sb2.method_name)
	dalvik_postcall(&d, &sb2);

	printString(env, obj, "sb2 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb2.comapreTo() = %d s> %s\n", res, s);
		(*env)->ReleaseStringUTFChars(env, str, s);
	}
	
	return (void *) res;
}

static void* sb3_comparetocase(JNIEnv *env, jobject obj, jobject str)
{
/*
	ddi_log_fmt("comparetocase\n")
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[1];
	args[0].l = str;
	dalvik_prepare(&d, &sb3, env);
	int res = (*env)->CallIntMethodA(env, obj, sb3.mid, args); 
	//ddi_log_fmt("success calling : %s\n", sb3.method_name)
	dalvik_postcall(&d, &sb3);

	printString(env, obj, "sb3 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb3.comapreToIgnoreCase() = %d s> %s\n", res, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}

	return (void *) res;
}

static void* sb7_indexof(JNIEnv *env, jobject obj, jobject str, jint i)
{
/*
	ddi_log_fmt("indexof\n")
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[2];
	args[0].l = str;
	args[1].i = i;
	dalvik_prepare(&d, &sb7, env);
	int res = (*env)->CallIntMethodA(env, obj, sb7.mid, args);
	//ddi_log_fmt("success calling : %s\n", sb7.method_name)
	dalvik_postcall(&d, &sb7);

	printString(env, obj, "sb7 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb7.indexOf() = %d (i=%d) %s\n", res, i, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}

	return (void *) res;
}

static void* sb11_indexof(JNIEnv *env, jobject obj, jobject str, jint i)
{
/*
	ddi_log_fmt("indexof\n")
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[2];
	args[0].l = str;
	args[1].i = i;
	dalvik_prepare(&d, &sb11, env);
	int res = (*env)->CallIntMethodA(env, obj, sb11.mid, args);
	//ddi_log_fmt("success calling : %s\n", sb11.method_name)
	dalvik_postcall(&d, &sb11);

	printString(env, obj, "sb11 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb11.indexOf() = %d (i=%d) %s\n", res, i, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}

	return (void *) res;
}

static void* sb10_startswith(JNIEnv *env, jobject obj, jobject str, jint i)
{
/*
	ddi_log_fmt("indexof\n")
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[2];
	args[0].l = str;
	args[1].i = i;
	dalvik_prepare(&d, &sb10, env);
	int res = (*env)->CallBooleanMethodA(env, obj, sb10.mid, args);
	//ddi_log_fmt("success calling : %s\n", sb10.method_name)
	dalvik_postcall(&d, &sb10);

	printString(env, obj, "sb10 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb10.startswith() = %d (i=%d) %s\n", res, i, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}

	return (void *) res;
}

static void* sb8_matches(JNIEnv *env, jobject obj, jobject str)
{
/*
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[1];
	args[0].l = str;
	dalvik_prepare(&d, &sb8, env);
	int res = (*env)->CallBooleanMethodA(env, obj, sb8.mid, args);
	//ddi_log_fmt("success calling : %s\n", sb8.method_name)
	dalvik_postcall(&d, &sb8);

	printString(env, obj, "sb8 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb8.matches() = %d %s\n", res, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}

	return (void *) res;
}

static void* sb13_equalsIgnoreCase(JNIEnv *env, jobject obj, jobject str)
{
/*
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[1];
	args[0].l = str;
	dalvik_prepare(&d, &sb13, env);
	int res = (*env)->CallBooleanMethodA(env, obj, sb13.mid, args);
	//ddi_log_fmt("success calling : %s\n", sb13.method_name)
	dalvik_postcall(&d, &sb13);

	printString(env, obj, "sb13 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb13.equalsIgnoreCase() = %d %s\n", res, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}

	return (void *) res;
}

static void* sb14_contentEquals(JNIEnv *env, jobject obj, jobject str)
{
/*
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[1];
	args[0].l = str;
	dalvik_prepare(&d, &sb14, env);
	int res = (*env)->CallBooleanMethodA(env, obj, sb14.mid, args);
	//ddi_log_fmt("success calling : %s\n", sb14.method_name)
	dalvik_postcall(&d, &sb14);

	printString(env, obj, "sb14 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb14.contentEquals() = %d %s\n", res, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}

	return (void *) res;
}

static void* sb9_endswith(JNIEnv *env, jobject obj, jobject str)
{
/*
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/

	jvalue args[1];
	args[0].l = str;
	dalvik_prepare(&d, &sb9, env);
	int res = (*env)->CallBooleanMethodA(env, obj, sb9.mid, args);
	//ddi_log_fmt("success calling : %s\n", sb9.method_name)
	dalvik_postcall(&d, &sb9);

	printString(env, obj, "sb9 = "); 

	char *s = (*env)->GetStringUTFChars(env, str, 0);
	if (s) {
		ddi_log_fmt("sb9.endswith() = %d %s\n", res, s);
		(*env)->ReleaseStringUTFChars(env, str, s); 
	}

	return (void *) res;
}


static void* sb6_contains(JNIEnv *env, jobject obj, jobject str)
{
/*
	ddi_log_fmt("contains\n")
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
*/
dalvik_dump_class(&d, "");

	jvalue args[1];
	args[0].l = str;
	dalvik_prepare(&d, &sb6, env);
	int res = (*env)->CallBooleanMethodA(env, obj, sb6.mid, args);
	//ddi_log_fmt("success calling : %s\n", sb6.method_name)
	dalvik_postcall(&d, &sb6);

	printString(env, obj, "sb6 = "); 

	return (void *) res;
}

static void* sb5_getmethod(JNIEnv *env, jobject obj, jobject str, jobject cls)
{
/*
	ddi_log_fmt("getmethod\n")
	ddi_log_fmt("env = 0x%x\n", env)
	ddi_log_fmt("obj = 0x%x\n", obj)
	ddi_log_fmt("str = 0x%x\n", str)
	ddi_log_fmt("cls = 0x%x\n", cls)
*/

	jvalue args[2];
	args[0].l = str;
	args[1].l = cls;
	dalvik_prepare(&d, &sb5, env);
	void *res = (*env)->CallObjectMethodA(env, obj, sb5.mid, args); 
	ddi_log_fmt("success calling : %s\n", sb5.method_name);
	dalvik_postcall(&d, &sb5);

	if (str) {
		char *s = (*env)->GetStringUTFChars(env, str, 0);
		if (s) {
			ddi_log_fmt("sb5.getmethod = %s\n", s);
			(*env)->ReleaseStringUTFChars(env, str, s); 
		}
	}

	return (void *) res;
}

void do_patch()
{
	ddi_log_fmt("setting hooks on String/String buffer methods ...\n");

	dalvik_hook_setup(&sb1, "Ljava/lang/StringBuffer;", 
            "toString",
            "()Ljava/lang/String;", 1, sb1_tostring);
	dalvik_hook(&d, &sb1);

	dalvik_hook_setup(&sb20, "Ljava/lang/StringBuilder;", 
            "toString", 
            "()Ljava/lang/String;", 1, sb20_tostring);
	dalvik_hook(&d, &sb20);

	dalvik_hook_setup(&sb2, "Ljava/lang/String;",
            "compareTo",
            "(Ljava/lang/String;)I", 2, sb2_compareto);
	dalvik_hook(&d, &sb2);

	dalvik_hook_setup(&sb3, "Ljava/lang/String;",
            "compareToIgnoreCase",
            "(Ljava/lang/String;)I", 2, sb3_comparetocase);
	dalvik_hook(&d, &sb3);

	dalvik_hook_setup(&sb13, "Ljava/lang/String;",
            "equalsIgnoreCase",
            "(Ljava/lang/String;)Z", 2, sb13_equalsIgnoreCase);
	dalvik_hook(&d, &sb13);
	
	dalvik_hook_setup(&sb6, "Ljava/lang/String;",
            "contains", 
            "(Ljava/lang/CharSequence;)Z", 2, sb6_contains);
	dalvik_hook(&d, &sb6);

	dalvik_hook_setup(&sb14, "Ljava/lang/String;",
            "contentEquals",
            "(Ljava/lang/StringBuffer;)Z", 2, sb14_contentEquals);
	dalvik_hook(&d, &sb14);

	dalvik_hook_setup(&sb7, "Ljava/lang/String;",
            "indexOf",
            "(Ljava/lang/String;I)I", 3, sb7_indexof);
	dalvik_hook(&d, &sb7);
	
	dalvik_hook_setup(&sb11, "Ljava/lang/StringBuffer;",
            "indexOf", 
            "(Ljava/lang/String;I)I", 3, sb11_indexof);
	dalvik_hook(&d, &sb11);
	
	dalvik_hook_setup(&sb9, "Ljava/lang/String;",
            "endsWith",
            "(Ljava/lang/String;)Z", 2, sb9_endswith);
	dalvik_hook(&d, &sb9);
	
	dalvik_hook_setup(&sb10, "Ljava/lang/String;",
            "startsWith",
            "(Ljava/lang/String;I)Z", 3, sb10_startswith);
	dalvik_hook(&d, &sb10);
	
	dalvik_hook_setup(&sb8, "Ljava/lang/String;",
            "matches",
            "(Ljava/lang/String;)Z", 2, sb8_matches);
	dalvik_hook(&d, &sb8);

	dalvik_hook_setup(&sb5, "Ljava/lang/Class;",
            "getMethod",
            "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;", 3, sb5_getmethod);
	dalvik_hook(&d, &sb5);
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
	    // insert hooks
	    do_patch();
	    
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
