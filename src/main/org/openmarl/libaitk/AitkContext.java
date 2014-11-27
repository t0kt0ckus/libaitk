/*
    Aitk: Android Instrumentation Toolkit.

    Based on crmulliner's adbi/ddi projects:
    [https://github.com/crmulliner/adbi]
    [https://github.com/crmulliner/ddi]

    <t0kt0ckus@gmail.com>
    (C) 2014

    License GPLv3
 */
package org.openmarl.libaitk;

import android.content.Context;
import android.util.Log;

import org.openmarl.susrv.LibSusrv;
import org.openmarl.susrv.SuShell;
import org.openmarl.susrv.SuShellInvalidatedException;
import org.openmarl.susrv.SuShellLifecycleObserver;

/**
 * Aitk injection API.
 *
 * <p>This provides <i>Collin's Dynamic Dalvik Instrumentation Toolkit for Android</i> as a simple
 * Java API.
 * </p>
 *
 * @author t0kt0ckus
 */
public class AitkContext  {

    public static final String AITK_DIR="/data/local/tmp/aitk";
    public static final String AITK_LIBS_DIR="/data/local/tmp/aitk/libs";
    public static final String AITK_LOGS_DIR="/data/local/tmp/aitk/logs";

    public static final String AITK_LIB_ADBI_TEST = "libadbi_test";
    public static final String AITK_LIB_DDI_TEST = "libddi_test";
    public static final String AITK_LIB_DUMP_ALL = "libaitk_dumpall";
    public static final String AITK_LIB_DUMP_CLASS = "libaitk_dumpclass";
    public static final String AITK_LIB_SEND_RAW_PDU = "libaitk_send_raw_pdu";
    public static final String AITK_CLASSES_DEX = "libaitk_classes";

    private static AitkContext _instance;

    private SuShell mSuShell;

    private AitkContext(SuShell shell)
    {
        mSuShell = shell;
        mSuShell.importExecutable("hijack", "hijack");

        initAitkDirs();
        defineBaseInjectionLibs();
        try {
            mSuShell.exec("chmod 0777 /data/dalvik-cache");
            mSuShell.exec("rm /data/dalvik-cache/data@local@tmp@libaitk_classes.dex");
        }
        catch(SuShellInvalidatedException e) {
            Log.e(TAG, e.toString()); // should not happen
        }
    }

    /**
     *
     * @param ctx
     * @return
     */
    public static AitkContext getInstance(Context ctx)
    {
        if (_instance == null) {
            SuShell suShell = SuShell.getInstance(ctx, true);
            if (suShell != null) {
                _instance = new AitkContext(suShell);
            }
            else {
                Log.e(TAG, "Failed to initialize Aitk context (no SU shell session)");
            }
        }

        return _instance;
    }

    public static AitkContext getInstance() {
        return _instance;
    }

    /**d
     *
     * @param libname
     * @return
     * @throws SuShellInvalidatedException
     */
    public boolean defineInjectionLib(String libname) throws SuShellInvalidatedException
    {
        String libFilePath = String.format("%s/%s.so", AITK_LIBS_DIR, libname);
        if (mSuShell.cpa(libname, libFilePath, 0644) == 0) {
            Log.i(TAG, String.format("Defined injection library <%s> as: %s",
                    libname, libFilePath));
            return true;
        } else
            Log.e(TAG, String.format("Failed to defined injection library <%s> as: %s",
                    libname, libFilePath));

        return false;
    }

    public boolean defineInjectionDex(String dexname) throws SuShellInvalidatedException
    {
        String libFilePath = String.format("%s/%s.dex", AITK_LIBS_DIR, dexname);
        if (mSuShell.cpa(dexname, libFilePath, 0644) == 0) {
            Log.i(TAG, String.format("Defined injection dex <%s> as: %s",
                    dexname, libFilePath));
            return true;
        } else
            Log.e(TAG, String.format("Failed to defined injection dex <%s> as: %s",
                    dexname, libFilePath));

        return false;
    }

    /**
     *
     * @param pid
     * @param libname
     * @return
     * @throws SuShellInvalidatedException
     */
    public boolean inject(int pid, String libname) throws SuShellInvalidatedException
    {
        String libFilePath = String.format("%s/%s.so", AITK_LIBS_DIR, libname);
        String cmd_hijack = String.format("hijack -d -p %d -l %s", pid, libFilePath);
        Log.d(TAG, String.format("hijack: %s", cmd_hijack));

        // FIXME: hijack returns 0 when injection lib unavailable
        int rval = mSuShell.exec(cmd_hijack);
        if (rval != 0)
        {
            Log.e(TAG, String.format("Failed to inject lib/marl/research/projects/Aitk/app/libsrary <%s> into process with PID %d",
                    libname, pid));
            return false;
        }

        Log.i(TAG, String.format("Injected library <%s> into process with PID %d", libname, pid));
        return true;
    }

    /**
     *
     * @param appname
     * @param libname
     * @return
     * @throws SuShellInvalidatedException
     */
    public boolean inject(String appname, String libname) throws SuShellInvalidatedException
    {
        int pid = LibSusrv.getpid(appname);
        if (pid > 0) {
            return inject(pid, libname);
        }

        return false;
    }

    public void release() {
        mSuShell.exit();
    }

    private boolean defineBaseInjectionLibs()
    {
        try {
            return defineInjectionLib(AITK_LIB_ADBI_TEST)
                    && defineInjectionLib(AITK_LIB_DDI_TEST)
                    && defineInjectionLib(AITK_LIB_DUMP_ALL)
                    && defineInjectionLib(AITK_LIB_DUMP_CLASS)
                    && defineInjectionLib(AITK_LIB_SEND_RAW_PDU)
                    && defineInjectionDex(AITK_CLASSES_DEX);
        }
        catch(SuShellInvalidatedException e) {
            Log.e(TAG, e.toString()); // this should not happen
        }

        return false;
    }

    private boolean initAitkDirs()
    {
        try {
            mSuShell.mkdir(AITK_DIR, 0755);
            mSuShell.mkdir(AITK_LIBS_DIR, 0755);
            mSuShell.mkdir(AITK_LOGS_DIR, 0777);
            return true;
        }
        catch (SuShellInvalidatedException e) {
            Log.e(TAG, e.toString()); // should not happen
        }

        return false;
    }

    private static final String TAG = AitkContext.class.getSimpleName();
}
