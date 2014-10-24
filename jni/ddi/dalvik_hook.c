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
 * (C) 2014
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "dexstuff.h"
#include "dalvik_hook.h"
#include "ddi_log.h"

int dalvik_hook_setup(struct dalvik_hook_t *h,
        char *cls,
        char *meth, 
        char *sig,
        int ns,
        void *func)
{
	if (!h)
		return 0;

	strcpy(h->clname, cls);
	strncpy(h->clnamep, cls+1, strlen(cls)-2);
	strcpy(h->method_name, meth);
	strcpy(h->method_sig, sig);
	h->n_iss = ns;
	h->n_rss = ns;
	h->n_oss = 0;
	h->native_func = func;

	h->sm = 0; // set by hand if needed

	h->af = 0x0100; // native, modify by hand if needed
	
	h->resolvm = 0; // don't resolve method on-the-fly, change by hand if needed

	h->debug_me = 1;

	return 1;
}

void* dalvik_hook(struct dexstuff_t *dex, struct dalvik_hook_t *h)
{
	if (h->debug_me)
		ddi_log_fmt("dalvik_hook: class %s\n", h->clname);
	
	void *target_cls = dex->dvmFindLoadedClass_fnPtr(h->clname);
	if (h->debug_me)
		ddi_log_fmt("class = 0x%x\n", target_cls);

    // FIXME: possible to print elsewhere ?
	// print class in logcat
	if (h->dump && dex && target_cls)
		dex->dvmDumpClass_fnPtr(target_cls, (void*)1);

	if (!target_cls) {
		if (h->debug_me)
			ddi_log_fmt("target_cls == 0\n");
		return (void*)0;
	}

	h->method = dex->dvmFindVirtualMethodHierByDescriptor_fnPtr(target_cls, h->method_name, h->method_sig);
	if (h->method == 0) {
		h->method = dex->dvmFindDirectMethodByDescriptor_fnPtr(target_cls, h->method_name, h->method_sig);
	}

	// constrcutor workaround, see "dalvik_prepare" below
	if (!h->resolvm) {
		h->cls = target_cls;
		h->mid = (void*)h->method;
	}

	if (h->debug_me)
		ddi_log_fmt("%s(%s) = 0x%x\n", 
                h->method_name, 
                h->method_sig, 
                h->method);

	if (h->method) {
		h->insns = h->method->insns;

		if (h->debug_me) {
			ddi_log_fmt("nativeFunc %x\n", h->method->nativeFunc);
		
			ddi_log_fmt(
                    "insSize = 0x%x  registersSize = 0x%x  outsSize = 0x%x\n",
                    h->method->insSize,
                    h->method->registersSize,
                    h->method->outsSize);
		}

		h->iss = h->method->insSize;
		h->rss = h->method->registersSize;
		h->oss = h->method->outsSize;
	
		h->method->insSize = h->n_iss;
		h->method->registersSize = h->n_rss;
		h->method->outsSize = h->n_oss;

		if (h->debug_me) {
			ddi_log_fmt("shorty %s\n", h->method->shorty);
			ddi_log_fmt("name %s\n", h->method->name);
			ddi_log_fmt("arginfo %x\n", h->method->jniArgInfo);
		}
		h->method->jniArgInfo = 0x80000000; // <--- also important
		if (h->debug_me) {
			ddi_log_fmt("noref %c\n", h->method->noRef);
			ddi_log_fmt("access %x\n", h->method->a);
		}
		h->access_flags = h->method->a;
		h->method->a = h->method->a | h->af; // make method native
		if (h->debug_me)
			ddi_log_fmt("access %x\n", h->method->a);
	
		dex->dvmUseJNIBridge_fnPtr(h->method, h->native_func);
		
		if (h->debug_me)
			ddi_log_fmt("patched %s to: 0x%x\n", 
                    h->method_name, h->native_func);

		return (void*)1;
	}
	else {
		if (h->debug_me)
			ddi_log_fmt("could NOT patch %s\n", h->method_name);
	}

	return (void*)0;
}

int dalvik_prepare(struct dexstuff_t *dex, 
        struct dalvik_hook_t *h,
        JNIEnv *env)
{
	// this seems to crash when hooking "constructors"

	if (h->resolvm) {
		h->cls = (*env)->FindClass(env, h->clnamep);
		if (h->debug_me)
			ddi_log_fmt("cls = 0x%x\n", h->cls);
		if (!h->cls)
			return 0;
		if (h->sm)
			h->mid = (*env)->GetStaticMethodID(env,
                    h->cls,
                    h->method_name,
                    h->method_sig);
		else
			h->mid = (*env)->GetMethodID(env,
                    h->cls,
                    h->method_name,
                    h->method_sig);
		if (h->debug_me)
			ddi_log_fmt("mid = 0x%x\n", h-> mid);
		if (!h->mid)
			return 0;
	}

	h->method->insSize = h->iss;
	h->method->registersSize = h->rss;
	h->method->outsSize = h->oss;
	h->method->a = h->access_flags;
	h->method->jniArgInfo = 0;
	h->method->insns = h->insns; 
}

void dalvik_postcall(struct dexstuff_t *dex, struct dalvik_hook_t *h)
{
	h->method->insSize = h->n_iss;
	h->method->registersSize = h->n_rss;
	h->method->outsSize = h->n_oss;

	//ddi_log_fmt("shorty %s\n", h->method->shorty)
	//ddi_log_fmt("name %s\n", h->method->name)
	//ddi_log_fmt("arginfo %x\n", h->method->jniArgInfo)
	h->method->jniArgInfo = 0x80000000;
	//ddi_log_fmt("noref %c\n", h->method->noRef)
	//ddi_log_fmt("access %x\n", h->method->a)
	h->access_flags = h->method->a;
	h->method->a = h->method->a | h->af;
	//ddi_log_fmt("access %x\n", h->method->a)

	dex->dvmUseJNIBridge_fnPtr(h->method, h->native_func);
	
	if (h->debug_me)
		ddi_log_fmt("patched BACK %s to: 0x%x\n", 
                h->method_name, h->native_func);
}

