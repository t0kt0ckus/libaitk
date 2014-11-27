package org.openmarl.libaitk;

import android.util.Log;

import com.android.internal.telephony.SmsMessageBase;
import com.android.internal.telephony.gsm.SmsMessage;

/**
 * Created by chris on 27/11/14.
 */
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
        }
    }

    private static final String TAG = "AITK_SEND_RAW_PDU";
}
