/*
    Aitk: Android Instrumentation Toolkit.

    Based on crmulliner's adbi/ddi projects:
    [https://github.com/crmulliner/adbi]
    [https://github.com/crmulliner/ddi]

    <t0kt0ckus@gmail.com>
    (C) 2014

    License GPLv3
 */
package org.openmarl.aitk;

import android.content.Context;
import android.util.Log;

import org.openmarl.susrv.LibSusrv;
import org.openmarl.susrv.SuShell;
import org.openmarl.susrv.SuShellInvalidatedException;

/**
 * Aitk injection API.
 *
 * <p>This provides <i>Collin's Dynamic Dalvik Instrumentation Toolkit for Android</i> as a simple
 * Java API.
 * </p>
 *
 * @author t0kt0ckus
 */
public class AitkContext {

    public static final String AITK_DIR="/data/local/tmp/aitk";
    public static final String AITK_LIBS_DIR="/data/local/tmp/aitk/libs";
    public static final String AITK_LOGS_DIR="/data/local/tmp/aitk/logs";

    public static final String AITK_LIB_ADBI_TEST = "libadbi_test";
    public static final String AITK_LIB_DDI_TEST = "libddi_test";
    public static final String AITK_LIB_DUMP_ALL = "libaitk_dumpall";
    public static final String AITK_LIB_DUMP_CLASS = "libaitk_dumpclass";

    private static AitkContext _instance;

    private SuShell mSuShell;

    private AitkContext(SuShell shell)
    {
        mSuShell = shell;
        mSuShell.importExecutable("hijack", "hijack");
        initDirs();
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
                defineStdlibs(_instance);
            }
            else {
                Log.e(TAG, "Failed to initialize SU Shell session");
            }
        }

        return _instance;
    }

    /**
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
        }
        else
            Log.e(TAG, String.format("Failed to defined injection library <%s> as: %s",
                    libname, libFilePath));
        return false;
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

        // FIXME: hijack returns 0 when injection lib on available
        int rval = mSuShell.exec(cmd_hijack);
        if (rval != 0)
        {
            Log.e(TAG, String.format("Failed to inject library <%s> into process with PID %d",
                    libname, pid));
            return false;
        }

        Log.i(TAG, String.format("Injected library <%s> into process with PID %d", libname, pid));
        return true;
    }

    private static boolean defineStdlibs(AitkContext aitk)
    {
        try {
            return  aitk.defineInjectionLib(AITK_LIB_ADBI_TEST)
                    && aitk.defineInjectionLib(AITK_LIB_DDI_TEST)
                    && aitk.defineInjectionLib(AITK_LIB_DUMP_ALL)
                    && aitk.defineInjectionLib(AITK_LIB_DUMP_CLASS);
        }
        catch (SuShellInvalidatedException e) {
            Log.e(TAG,
                    String.format("Failed to define AITK standard libs: %s", e.toString()));
            return false;
        }
    }

    private boolean initDirs()
    {
        try {
            mSuShell.mkdir(AITK_DIR, 0755);
            mSuShell.mkdir(AITK_LIBS_DIR, 0755);
            mSuShell.mkdir(AITK_LOGS_DIR, 0777);
            return true;
        }
        catch (SuShellInvalidatedException e) {
            Log.e(TAG,
                    String.format("Failed to init AITK standard dirs: %s", e.toString()));
        }

        return false;
    }

    private static final String TAG = AitkContext.class.getSimpleName();
}