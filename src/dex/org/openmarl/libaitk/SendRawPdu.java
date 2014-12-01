package org.openmarl.libaitk;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Binder;
import android.telephony.SmsManager;
import android.util.Log;

import com.android.internal.telephony.IccSmsInterfaceManager;
import com.android.internal.telephony.ImsSMSDispatcher;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.PhoneProxy;
import com.android.internal.telephony.SMSDispatcher;
import com.android.internal.telephony.cdma.CdmaSMSDispatcher;
import com.android.internal.telephony.gsm.GsmSMSDispatcher;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;

public class SendRawPdu {

    private static final String FIELD_ICC_SMS_INTERFACE_MANAGER = "mIccSmsInterfaceManager";
    private static final String FIELD_SMS_DISPATCHER = "mDispatcher";

    private Phone mPhone;
    private GsmSMSDispatcher mGsmSmsDispatcher;
    private CdmaSMSDispatcher mCdmaSmsDispatcher;

    public SendRawPdu() {
        mPhone = PhoneFactory.getDefaultPhone();
        if (mPhone != null) {
            if (mPhone instanceof PhoneProxy) {
                initInternalApis();

                if (mGsmSmsDispatcher != null)
                    //sendRawPdu("0643973069");
                    sendRawPdu("0641543723");
            }
            else Log.e(TAG, "Default phone is not a PhoneProxy instance !");
        }
        else Log.e(TAG, "Failed to get default phone !");
    }


    private void initInternalApis()
    {
        PhoneProxy phoneProxy = (PhoneProxy) mPhone;
        try {
            Field field_mIccSmsImgr =
                    PhoneProxy.class.getDeclaredField(FIELD_ICC_SMS_INTERFACE_MANAGER);
            field_mIccSmsImgr.setAccessible(true);
            Object iccSmsImgr = field_mIccSmsImgr.get(phoneProxy);

            if ((iccSmsImgr != null) && (iccSmsImgr instanceof IccSmsInterfaceManager)) {
                Field field_mDispatcher =
                        IccSmsInterfaceManager.class.getDeclaredField(FIELD_SMS_DISPATCHER);
                field_mDispatcher.setAccessible(true);
                Object smsDispatcher = field_mDispatcher.get(iccSmsImgr);

                if ((smsDispatcher != null) && (smsDispatcher instanceof ImsSMSDispatcher)) {
                    ImsSMSDispatcher imsSmsDispatcher = (ImsSMSDispatcher) smsDispatcher;

                    Field field_gsmDispatcher =
                            ImsSMSDispatcher.class.getDeclaredField("mGsmDispatcher");
                    field_gsmDispatcher.setAccessible(true);
                    mGsmSmsDispatcher =
                            (GsmSMSDispatcher) field_gsmDispatcher.get(imsSmsDispatcher);

                    Log.i(TAG, "Successfully acquired proper GsmSMSDispatcher instance");
                }
            }
        }
        catch (NoSuchFieldException e) {
            Log.e(TAG, String.format("Unexpected reflection error: %s", e.toString()));
        }
        catch (IllegalAccessException e) {
            Log.e(TAG, String.format("Unexpected reflection error: %s", e.toString()));
        }
    }

    public void sendRawPdu(String destination)
    {
        AitkSubmitPdu pdu = SubmitPduFactory.getSubmitPdu(destination,
                "test",
                true);

        Class class_SmsTracker = null;
        for (Class clazz : SMSDispatcher.class.getDeclaredClasses())
            if (clazz.getSimpleName().equals("SmsTracker"))
                class_SmsTracker = clazz;

        if (class_SmsTracker != null)
        {
            try
            {
                Method method_sendSms = GsmSMSDispatcher.class.getDeclaredMethod("sendSms",
                        class_SmsTracker);
                method_sendSms.setAccessible(true);
                Log.i(TAG, "Successfully resolved sendSms(SmsTracker)");

                Constructor init_SmsTracker =
                        class_SmsTracker.getDeclaredConstructor(HashMap.class, // data (pdu)
                                PendingIntent.class, // sent
                                PendingIntent.class, // delivery
                                PackageInfo.class, // app info (we "skip" this)
                                String.class, //destAddr
                                String.class); // format (3gpp)
                init_SmsTracker.setAccessible(true);

                HashMap<String, byte[]> data = new HashMap<String, byte[]>();
                data.put("pdu", pdu.encodedMessage);
                data.put("smsc", pdu.encodedScAddress);

                PackageManager pm = mPhone.getContext().getPackageManager();
                String[] packageNames = pm.getPackagesForUid(Binder.getCallingUid());
                PackageInfo appInfo = null;
                if (packageNames != null && packageNames.length > 0) {
                    try {
                        // XXX this is lossy- apps can share a UID
                        appInfo = pm.getPackageInfo(packageNames[0], PackageManager.GET_SIGNATURES);
                    }
                    catch (PackageManager.NameNotFoundException e) { }
                }

                Object smsTracker = init_SmsTracker.newInstance(data,
                        getSilentSmsSentStatusIntent(mPhone.getContext()),
                        getSilentSmsDeliveryStatusIntent(mPhone.getContext()),
                        appInfo,
                        destination,
                        "3gpp");

                if (smsTracker != null)
                {
                    method_sendSms.invoke(mGsmSmsDispatcher, smsTracker);

                    Log.i(TAG, "SMS should be sent ...");
                }
                else
                    Log.e(TAG, "Failed <init> SmsTracker instance !");
            }
            catch (NoSuchMethodException e) {
                Log.e(TAG, String.format("Unexpected reflection error: %s", e.toString()));
            } catch (IllegalAccessException e) {
                Log.e(TAG, String.format("Unexpected reflection error: %s", e.toString()));
            } catch (InstantiationException e) {
                Log.e(TAG, String.format("Unexpected reflection error: %s", e.toString()));
            } catch (InvocationTargetException e) {
                Log.e(TAG, String.format("Unexpected reflection error: %s", e.toString()));
            }
        }
    }

    private PendingIntent getSilentSmsSentStatusIntent(Context context) {
        Intent intent = new Intent();
        intent.setAction("org.openmarl.aitk.gsm.SilentSmsSent");
        return PendingIntent.getBroadcast(context, 0, intent, 0);
    }

    private PendingIntent getSilentSmsDeliveryStatusIntent(Context context) {
        Intent intent = new Intent();
        intent.setAction("org.openmarl.aitk.gsm.SilentSmsDelivery");
        return PendingIntent.getBroadcast(context, 0, intent, 0);
    }

    private static final String TAG = "AITK_GSM";
}
