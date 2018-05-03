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

import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.log.AlogMarker;

public class ModemInterface {

    private final String TAG = "AMTL";
    private final String MODULE = "ModemInterface";
    private int interfaceSerialFd = -1;
    private int baudrate = 115200;
    private String interfaceName = "";

    /* Load Amtl JNI library */
    static {
        System.loadLibrary("amtl_jni");
    }

    private native int OpenSerialTty(String jtty_name, int baudrate);

    private native int CloseSerial(int fd);

    public ModemInterface(String ifName) {
        AlogMarker.tAB("ModemInterface.ModemInterface", "0");
        this.interfaceName = ifName;
        AlogMarker.tAE("ModemInterface.ModemInterface", "0");
    }

    public void openInterface() throws ModemControlException {
        AlogMarker.tAB("ModemInterface.openInterface", "0");
        /* Check if modem interface is already open */
        if (this.interfaceSerialFd < 0) {
            /* Not open -> open it */
            this.interfaceSerialFd = this.OpenSerialTty(this.interfaceName, this.baudrate);
            if (this.interfaceSerialFd < 0) {
                String error = " : openInterface() failed " + this.interfaceName;
                Log.e(TAG, MODULE + error);
                throw new ModemControlException("Error while opening modem interface" + error);
            } else {
                Log.d(TAG, MODULE + " : succeed in opening modem interface " + this.interfaceName);
            }
        } else {
            Log.d(TAG, MODULE +  " : modem interface already open " + this.interfaceName);
        }
        AlogMarker.tAE("ModemInterface.openInterface", "0");
    }

    public void closeInterface() {
        AlogMarker.tAB("ModemInterface.closeInterface", "0");
        Log.d(TAG, MODULE + " : closeTty() ongoing " + this.interfaceName);
        this.CloseSerial(this.interfaceSerialFd);
        this.interfaceSerialFd = -1;
        AlogMarker.tAE("ModemInterface.closeInterface", "0");
    }
}
