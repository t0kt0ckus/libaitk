package org.openmarl.libaitk;

import java.util.Arrays;

/**
 * Created by chris on 27/11/14.
 */
public class AitkSubmitPdu {
    public byte[] encodedScAddress; // Null if not applicable.
    public byte[] encodedMessage;

    @Override
    public String toString() {
        return "SubmitPdu: encodedScAddress = "
                + Arrays.toString(encodedScAddress)
                + ", encodedMessage = "
                + Arrays.toString(encodedMessage);
    }

}
