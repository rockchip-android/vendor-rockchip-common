/* Android Modem Traces and Logs
 *
 * Copyright (C) Intel 2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Nicolae Natea <nicolaex.natea@intel.com>
 */

package com.intel.amtl.service;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import java.io.IOException;

public class LogCaptureService extends Service {
    private static Process sLogcatProc = null;
    private static final String TAG = "AMTL";
    private static final String MODULE = "LogCaptureService";
    private final IBinder mBinder = new LogCaptureLocalBinder();
    public static final String EXTRA_PARAMETERS = "parameters";

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, MODULE + ": onCreate");
    }

    private static boolean stopLogcatProcess() {

        if (sLogcatProc != null) {
            sLogcatProc.destroy();
            sLogcatProc = null;
        }

        return true;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, MODULE + ": onStartCommand");

        String command = intent.getStringExtra(EXTRA_PARAMETERS);

        stopLogcatProcess();
        try {
            Log.d(TAG, MODULE + ": executing command: " + command);
            sLogcatProc = Runtime.getRuntime().exec(command);
        } catch (IOException e) {
            Log.e(TAG, MODULE + ": Could not start logcat log");
            stopSelf();
        }

        return START_REDELIVER_INTENT;
    }

    @Override
    public IBinder onBind(Intent arg0) {
        Log.d(TAG, MODULE + ": onBind");
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.d(TAG, MODULE + ": onUnbind");
        return false;
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, MODULE + ": onDestroy");
        stopLogcatProcess();
    }

    public class LogCaptureLocalBinder extends Binder {
        private static final int WAITING_TIME = 67;

        LogCaptureService getService() {
            int counter = 0;
            while (LogCaptureService.this == null && counter < WAITING_TIME) {
                try {
                    Thread.sleep(30);
                    counter++;
                } catch (InterruptedException e) {
                    Log.e(TAG, "LogCaptureLocalBinder:getService: Interrupted Exception");
                }
            }
            return LogCaptureService.this;
        }
    }
}
