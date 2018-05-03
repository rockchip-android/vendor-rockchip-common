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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 */

package com.intel.amtl.modem.communication;

import android.util.Log;

import com.intel.amtl.log.AlogMarker;

public class IoctlWrapper {

    private final String TAG = "AMTL";
    private final String MODULE = "IoctlWrapper";
    private final int NONE = 0;
    private final int READ_IF = 1;
    private final int LDISC = 2;
    private final int FILE = 3;
    private final int USB = 4;
    private final String OCT_PATH_NONE = "OCT_PATH_NONE";
    private final String OCT_PATH_READ_IF = "OCT_PATH_READ_IF";
    private final String OCT_PATH_LDISC = "OCT_PATH_LDISC";
    private final String OCT_PATH_FILE = "OCT_PATH_FILE";
    private final String OCT_PATH_USB = "OCT_PATH_USB";

    /* Load IOCTL JNI library */
    static {
        System.loadLibrary("ioctl_jni");
    }
    private native int GetOctPath(String device);

    public String getOctDriverPath() {
        AlogMarker.tAB(MODULE + ".getOctDriverPath", "0");
        int path = GetOctPath("/dev/oct");
        AlogMarker.tAE(MODULE + ".getOctDriverPath", "0");
        switch (path) {
            case NONE:
            return OCT_PATH_NONE;
            case READ_IF:
            return OCT_PATH_READ_IF;
            case LDISC:
            return OCT_PATH_LDISC;
            case FILE:
            return OCT_PATH_FILE;
            case USB:
            return OCT_PATH_USB;
            default:
            return "ERROR";
        }
    }
}
