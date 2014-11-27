package org.openmarl.libaitk;

import android.content.Context;
import android.os.AsyncTask;

/**
 * Created by chris on 14/11/14.
 */
public class AitkContextAsyncInit extends AsyncTask<Void,Void,AitkContext> {

    private Context mContext;

    public AitkContextAsyncInit(Context ctx) {
        mContext = ctx;
    }


    @Override
    protected AitkContext doInBackground(Void... params) {
        return AitkContext.getInstance(mContext);
    }

    @Override
    protected void onPostExecute(AitkContext aitkContext) {
        super.onPostExecute(aitkContext);

        if (mContext instanceof  AitkContextObserver) {
            ((AitkContextObserver) mContext).onContextReady(aitkContext);
        }
    }
}
