package org.openmarl.libaitk;

/**
 * Created by chris on 13/11/14.
 */
public interface AitkContextObserver {

    public void onContextReady(AitkContext ctx);

    public void onContextDisconnected();
}
