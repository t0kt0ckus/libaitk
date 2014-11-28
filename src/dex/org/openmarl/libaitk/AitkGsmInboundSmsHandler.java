package org.openmarl.libaitk;

import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.android.internal.telephony.SmsMessageBase;
import com.android.internal.telephony.gsm.SmsMessage;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class AitkGsmInboundSmsHandler {

    public AitkGsmInboundSmsHandler(SmsMessageBase smsb)
    {
        SmsMessage sms = (SmsMessage) smsb;

        if (sms.isTypeZero())
        {
            Log.w(TAG,
                    String.format("SMS-Type 0 from SMSC %s (%s)",
                            smsb.getServiceCenterAddress(),
                            smsb.getOriginatingAddress()));

            Context appContext = getApplicationContext();
            if (appContext != null)
            {
                Intent onSilentSmsReceived = new Intent();
                onSilentSmsReceived.setAction("org.openmarl.aitk.gsm.SilentSmsReceived");

                onSilentSmsReceived.putExtra("org.openmarl.aitk.gsm.SMSC",
                        smsb.getServiceCenterAddress());
                onSilentSmsReceived.putExtra("org.openmarl.aitk.gsm.ORIGIN",
                        smsb.getOriginatingAddress());
                onSilentSmsReceived.putExtra("org.openmarl.aitk.gsm.BODY",
                        smsb.getMessageBody());
                onSilentSmsReceived.putExtra("org.openmarl.aitk.gsm.PDU",
                        smsb.getPdu());
                onSilentSmsReceived.putExtra("org.openmarl.aitk.gsm.TP_PID",
                        smsb.getProtocolIdentifier());

                appContext.sendBroadcast(onSilentSmsReceived);
            }
            else
                Log.e(TAG, "Failed to retreive a valid application context !");
        }
    }

    private Application getApplicationContext()
    {
        Application appContext = null;

        try
        {
            final Class<?> activityThreadClass =
                    Class.forName("android.app.ActivityThread");

            final Method fnCurrentApplication =
                    activityThreadClass.getMethod("currentApplication");

            appContext = (Application) fnCurrentApplication.invoke(null);
        }
        catch (final ClassNotFoundException e) {
            Log.e(TAG, e.toString());
        }
        catch (final NoSuchMethodException e) {
            Log.e(TAG, e.toString());
        }
        catch (final IllegalArgumentException e) {
            Log.e(TAG, e.toString());
        }
        catch (final IllegalAccessException e) {
            Log.e(TAG, e.toString());
        }
        catch (final InvocationTargetException e) {
            Log.e(TAG, e.toString());
        }

        return appContext;
    }

    private static final String TAG = "AITK_GSM";
}
