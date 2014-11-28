package org.openmarl.libaitk;


import android.telephony.PhoneNumberUtils;
import android.util.Log;

import com.android.internal.telephony.EncodeException;
import com.android.internal.telephony.GsmAlphabet;
import com.android.internal.telephony.SmsHeader;

import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;

import static com.android.internal.telephony.SmsConstants.ENCODING_16BIT;
import static com.android.internal.telephony.SmsConstants.ENCODING_7BIT;
import static com.android.internal.telephony.SmsConstants.ENCODING_UNKNOWN;
import static com.android.internal.telephony.SmsConstants.MAX_USER_DATA_BYTES;
import static com.android.internal.telephony.SmsConstants.MAX_USER_DATA_BYTES_WITH_HEADER;
import static com.android.internal.telephony.SmsConstants.MAX_USER_DATA_SEPTETS;

public class SubmitPduFactory {

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message using the
     * specified encoding.
     *
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns null on encode error.
     * @hide
     */
    public static AitkSubmitPdu getSubmitPdu(String destinationAddress,
                                             String message,
                                             boolean statusReportRequested)
    {
        String scAddress = null;
        byte[] header = null;
        int encoding = ENCODING_UNKNOWN;
        int languageTable = 0;
        int languageShiftTable = 0;

        // Perform null parameter checks.
        if (message == null || destinationAddress == null) {
            return null;
        }

        if (encoding == ENCODING_UNKNOWN) {
            // Find the best encoding to use
            GsmAlphabet.TextEncodingDetails ted = calculateLength(message, false);
            encoding = ted.codeUnitSize;
            languageTable = ted.languageTable;
            languageShiftTable = ted.languageShiftTable;

            if (encoding == ENCODING_7BIT &&
                    (languageTable != 0 || languageShiftTable != 0)) {
                if (header != null) {
                    SmsHeader smsHeader = SmsHeader.fromByteArray(header);
                    if (smsHeader.languageTable != languageTable
                            || smsHeader.languageShiftTable != languageShiftTable) {
                        Log.w(TAG, "Updating language table in SMS header: "
                                + smsHeader.languageTable + " -> " + languageTable + ", "
                                + smsHeader.languageShiftTable + " -> " + languageShiftTable);
                        smsHeader.languageTable = languageTable;
                        smsHeader.languageShiftTable = languageShiftTable;
                        header = SmsHeader.toByteArray(smsHeader);
                    }
                } else {
                    SmsHeader smsHeader = new SmsHeader();
                    smsHeader.languageTable = languageTable;
                    smsHeader.languageShiftTable = languageShiftTable;
                    header = SmsHeader.toByteArray(smsHeader);
                }
            }
        }

        AitkSubmitPdu ret = new AitkSubmitPdu();
        // MTI = SMS-SUBMIT, UDHI = header != null
        byte mtiByte = (byte)(0x01 | (header != null ? 0x40 : 0x00));
        ByteArrayOutputStream bo = getSubmitPduHead(
                scAddress, destinationAddress, mtiByte,
                statusReportRequested, ret);

        // User Data (and length)
        byte[] userData;
        try {
            if (encoding == ENCODING_7BIT) {
                userData = GsmAlphabet.stringToGsm7BitPackedWithHeader(message, header,
                        languageTable, languageShiftTable);
            } else { //assume UCS-2
                try {
                    userData = encodeUCS2(message, header);
                } catch(UnsupportedEncodingException uex) {
                    Log.e(TAG,
                            "Implausible UnsupportedEncodingException ",
                            uex);
                    return null;
                }
            }
        } catch (EncodeException ex) {
            // Encoding to the 7-bit alphabet failed. Let's see if we can
            // send it as a UCS-2 encoded message
            try {
                userData = encodeUCS2(message, header);
                encoding = ENCODING_16BIT;
            } catch(UnsupportedEncodingException uex) {
                Log.e(TAG,
                        "Implausible UnsupportedEncodingException ",
                        uex);
                return null;
            }
        }

        if (encoding == ENCODING_7BIT) {
            if ((0xff & userData[0]) > MAX_USER_DATA_SEPTETS) {
                // Message too long
                Log.e(TAG, "Message too long (" + (0xff & userData[0]) + " septets)");
                return null;
            }
            // TP-Data-Coding-Scheme
            // Default encoding, uncompressed
            // To test writing messages to the SIM card, change this value 0x00
            // to 0x12, which means "bits 1 and 0 contain message class, and the
            // class is 2". Note that this takes effect for the sender. In other
            // words, messages sent by the phone with this change will end up on
            // the receiver's SIM card. You can then send messages to yourself
            // (on a phone with this change) and they'll end up on the SIM card.
            bo.write(0x00);
        } else { // assume UCS-2
            if ((0xff & userData[0]) > MAX_USER_DATA_BYTES) {
                // Message too long
                Log.e(TAG, "Message too long (" + (0xff & userData[0]) + " bytes)");
                return null;
            }
            // TP-Data-Coding-Scheme
            // UCS-2 encoding, uncompressed
            bo.write(0x08);
        }

        // (no TP-Validity-Period)
        bo.write(userData, 0, userData.length);
        ret.encodedMessage = bo.toByteArray();
        return ret;
    }

    /**
     * Calculate the number of septets needed to encode the message.
     *
     * @param msgBody the message to encode
     * @param use7bitOnly ignore (but still count) illegal characters if true
     * @return TextEncodingDetails
     */
    public static GsmAlphabet.TextEncodingDetails calculateLength(CharSequence msgBody,
                                                                  boolean use7bitOnly) {
        GsmAlphabet.TextEncodingDetails ted = GsmAlphabet.countGsmSeptets(msgBody, use7bitOnly);
        if (ted == null) {
            ted = new GsmAlphabet.TextEncodingDetails();
            int octets = msgBody.length() * 2;
            ted.codeUnitCount = msgBody.length();
            if (octets > MAX_USER_DATA_BYTES) {
                ted.msgCount = (octets + (MAX_USER_DATA_BYTES_WITH_HEADER - 1)) /
                        MAX_USER_DATA_BYTES_WITH_HEADER;
                ted.codeUnitsRemaining = ((ted.msgCount *
                        MAX_USER_DATA_BYTES_WITH_HEADER) - octets) / 2;
            } else {
                ted.msgCount = 1;
                ted.codeUnitsRemaining = (MAX_USER_DATA_BYTES - octets)/2;
            }
            ted.codeUnitSize = ENCODING_16BIT;
        }
        return ted;
    }
    /**
     * Packs header and UCS-2 encoded message. Includes TP-UDL & TP-UDHL if necessary
     *
     * @return encoded message as UCS2
     * @throws UnsupportedEncodingException
     */
    private static byte[] encodeUCS2(String message, byte[] header)
            throws UnsupportedEncodingException {
        byte[] userData, textPart;
        textPart = message.getBytes("utf-16be");

        if (header != null) {
            // Need 1 byte for UDHL
            userData = new byte[header.length + textPart.length + 1];

            userData[0] = (byte)header.length;
            System.arraycopy(header, 0, userData, 1, header.length);
            System.arraycopy(textPart, 0, userData, header.length + 1, textPart.length);
        }
        else {
            userData = textPart;
        }
        byte[] ret = new byte[userData.length+1];
        ret[0] = (byte) (userData.length & 0xff );
        System.arraycopy(userData, 0, ret, 1, userData.length);
        return ret;
    }
    /**
     * Create the beginning of a SUBMIT PDU.  This is the part of the
     * SUBMIT PDU that is common to the two versions of {@link #getSubmitPdu},
     * one of which takes a byte array and the other of which takes a
     * <code>String</code>.
     *
     * @param scAddress Service Centre address. null == use default
     * @param destinationAddress the address of the destination for the message
     * @param mtiByte
     * @param ret <code>SubmitPdu</code> containing the encoded SC
     *        address, if applicable, and the encoded message
     */
    private static ByteArrayOutputStream getSubmitPduHead(
            String scAddress, String destinationAddress, byte mtiByte,
            boolean statusReportRequested, AitkSubmitPdu ret) {
        ByteArrayOutputStream bo = new ByteArrayOutputStream(
                MAX_USER_DATA_BYTES + 40);

        // SMSC address with length octet, or 0
        if (scAddress == null) {
            ret.encodedScAddress = null;
        } else {
            ret.encodedScAddress = PhoneNumberUtils.networkPortionToCalledPartyBCDWithLength(
                    scAddress);
        }

        // TP-Message-Type-Indicator (and friends)
        if (statusReportRequested) {
            // Set TP-Status-Report-Request bit.
            mtiByte |= 0x20;
        }
        bo.write(mtiByte);

        // space for TP-Message-Reference
        bo.write(0);

        byte[] daBytes;

        daBytes = PhoneNumberUtils.networkPortionToCalledPartyBCD(destinationAddress);

        // destination address length in BCD digits, ignoring TON byte and pad
        // TODO Should be better.
        bo.write((daBytes.length - 1) * 2
                - ((daBytes[daBytes.length - 1] & 0xf0) == 0xf0 ? 1 : 0));

        // destination address
        bo.write(daBytes, 0, daBytes.length);

        // TP-Protocol-Identifier
        //
        // instead of 0
        // bo.write(0);
        //
        bo.write(0x40); // we set it to Type 0 - SilentSMS

        return bo;
    }

    private static final String TAG = "AITG_GSM";
}
