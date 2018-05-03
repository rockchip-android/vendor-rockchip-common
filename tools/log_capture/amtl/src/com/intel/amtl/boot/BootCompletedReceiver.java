/* Android AMTL
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
 */

package com.android.amtl.boot;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.intel.amtl.helper.FileOperations;
import com.intel.amtl.tracing.LogcatTraces;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.StoredSettings;


public class BootCompletedReceiver extends BroadcastReceiver {

    private final String TAG = "AMTL";
    private final String MODULE = "BootCompletedReceiver";
    private AlogMarker m = new AlogMarker();

    @Override
    public void onReceive(Context context, Intent intent) {
        m.tAB("BootCompletedReceiver.onReceive", "0");
        String action = intent.getAction();

        if (action.equals(Intent.ACTION_BOOT_COMPLETED)) {
            Log.d(TAG, MODULE + ": BOOT COMPLETED");

            LogcatTraces.relaunchLogcatProcess(context);
        }
        m.tAE("BootCompletedReceiver.onReceive", "0");
    }
}
