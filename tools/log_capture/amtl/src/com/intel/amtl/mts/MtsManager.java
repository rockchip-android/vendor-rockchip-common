/* Android Modem Traces and Logs
 *
 * Copyright (C) Intel 2013
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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 */

package com.intel.amtl.mts;

import android.os.SystemProperties;
import android.util.Log;

import com.intel.amtl.log.AlogMarker;

import java.io.IOException;

public class MtsManager {

    private static final String TAG = "AMTL";
    private static final String MODULE = "MtsManager";
    private static final String EMPTY_STRING = "";
    // time to let mts unconfigure the ldiscs before stopping it
    private static final int PTI_KILL_WAIT = 75;

    public static final Runtime rtm = java.lang.Runtime.getRuntime();

    public static String getMtsInput() {
        AlogMarker.tAB("MtsManager.getMtsInput", "0");
        AlogMarker.tAE("MtsManager.getMtsInput", "0");
        return SystemProperties.get(MtsProperties.getInput(), EMPTY_STRING);
    }

    public static String getMtsOutput() {
        AlogMarker.tAB("MtsManager.getMtsOutput", "0");
        AlogMarker.tAE("MtsManager.getMtsOutput", "0");
        return SystemProperties.get(MtsProperties.getOutput(), EMPTY_STRING);
    }

    public static String getMtsOutputType() {
        AlogMarker.tAB("MtsManager.getMtsOutputType", "0");
        AlogMarker.tAE("MtsManager.getMtsOutputType", "0");
        return SystemProperties.get(MtsProperties.getOutputType(), EMPTY_STRING);
    }

    public static String getMtsRotateNum() {
        AlogMarker.tAB("MtsManager.getMtsRotateNum", "0");
        AlogMarker.tAE("MtsManager.getMtsRotateNum", "0");
        return SystemProperties.get(MtsProperties.getRotateNum(), EMPTY_STRING);
    }

    public static String getMtsRotateSize() {
        AlogMarker.tAB("MtsManager.getMtsRotateSize", "0");
        AlogMarker.tAE("MtsManager.getMtsRotateSize", "0");
        return SystemProperties.get(MtsProperties.getRotateSize(), EMPTY_STRING);
    }

    public static String getMtsInterface() {
        AlogMarker.tAB("MtsManager.getMtsInterface", "0");
        AlogMarker.tAE("MtsManager.getMtsInterface", "0");
        return SystemProperties.get(MtsProperties.getInterface(), EMPTY_STRING);
    }

    public static String getMtsBufferSize() {
        AlogMarker.tAB("MtsManager.getMtsBufferSize", "0");
        AlogMarker.tAE("MtsManager.getMtsBufferSize", "0");
        return SystemProperties.get(MtsProperties.getBufferSize(), EMPTY_STRING);
    }

    public static void printMtsProperties() {
        AlogMarker.tAB("MtsManager.printMtsProperties", "0");
        Log.d(TAG, MODULE + ": ========= MTS CONFIGURATION =========");
        Log.d(TAG, MODULE + ": INPUT = " + getMtsInput());
        Log.d(TAG, MODULE + ": OUTPUT = " + getMtsOutput());
        Log.d(TAG, MODULE + ": OUTPUT TYPE = " + getMtsOutputType());
        Log.d(TAG, MODULE + ": ROTATE NUM = " + getMtsRotateNum());
        Log.d(TAG, MODULE + ": ROTATE SIZE = " + getMtsRotateSize());
        Log.d(TAG, MODULE + ": INTERFACE = " + getMtsInterface());
        Log.d(TAG, MODULE + ": BUFFER SIZE = " + getMtsBufferSize());
        Log.d(TAG, MODULE + ": STATUS = " + getMtsState());
        Log.d(TAG, MODULE + ": =======================================");
        AlogMarker.tAE("MtsManager.printMtsProperties", "0");
    }

    public static String getMtsState() {
        AlogMarker.tAB("MtsManager.getMtsState", "0");
        AlogMarker.tAE("MtsManager.getMtsState", "0");
        return SystemProperties.get(MtsProperties.getStatus());
    }

    public static String getMtsMode() {
        AlogMarker.tAB("MtsManager.getMtsMode", "0");
        String mode = "oneshot";
        if (SystemProperties.get(MtsProperties.getPersistentService()).equals("1")) {
            mode = "persistent";
        }
        AlogMarker.tAE("MtsManager.getMtsMode", "0");
        return mode;
    }

    /* Start mts service */
    public static void startService(String service) {
        AlogMarker.tAB("MtsManager.startService", "0");
        if (service.equals("persistent")) {
            startMtsPersistent();
        } else if (service.equals("oneshot")) {
            startMtsOneshot();
        } else {
            Log.d(TAG, MODULE + ": cannot start mts, wrong mts mode");
        }
        AlogMarker.tAE("MtsManager.startService", "0");
    }

    /* Start mts persistent */
    private static void startMtsPersistent() {
        AlogMarker.tAB("MtsManager.startMtsPersistent", "0");
        String persService = MtsProperties.getPersistentService();
        Log.d(TAG, MODULE + ": starting " + persService + " persistent");
        SystemProperties.set(persService, "1");
        AlogMarker.tAE("MtsManager.startMtsPersistent", "0");
    }

    /* Start mts oneshot */
    private static void startMtsOneshot() {
        AlogMarker.tAB("MtsManager.startMtsOneshot", "0");
        String oneshotService = MtsProperties.getOneshotService();
        SystemProperties.set(oneshotService, "0");
        Log.d(TAG, MODULE + ": starting " + oneshotService + " oneshot");
        SystemProperties.set(oneshotService, "1");
        AlogMarker.tAE("MtsManager.startMtsOneshot", "0");
    }

    /* Stop the current service */
    public static void stopServices() {
        AlogMarker.tAB("MtsManager.stopServices", "0");
        try {
            if (getMtsOutputType().equals("k")) {
                Log.d(TAG, MODULE + ": Sending signal to unconfigure ldiscs");
                rtm.exec("start pti_sigusr1");
                android.os.SystemClock.sleep(PTI_KILL_WAIT);
            }
        } catch (IOException e) {
            Log.e(TAG, MODULE + ": can't send sigusr1 signal " + e);
        } finally {
             stopMtsPersistent();
             stopMtsOneshot();
        }
        AlogMarker.tAE("MtsManager.stopServices", "0");
    }

    /* Stop mts persistent */
    private static void stopMtsPersistent() {
        AlogMarker.tAB("MtsManager.stopMtsPersistent", "0");
        String persService = MtsProperties.getPersistentService();
        Log.d(TAG, MODULE + ": stopping " + persService + " persistent");
        SystemProperties.set(persService, "0");
        AlogMarker.tAE("MtsManager.stopMtsPersistent", "0");
    }

    /* Stop mts oneshot */
    private static void stopMtsOneshot() {
        AlogMarker.tAB("MtsManager.stopMtsOneshot", "0");
        String oneshotService = MtsProperties.getOneshotService();
        Log.d(TAG, MODULE + ": stopping " + oneshotService + " oneshot");
        SystemProperties.set(oneshotService, "0");
        AlogMarker.tAE("MtsManager.stopMtsOneshot", "0");
    }
}
