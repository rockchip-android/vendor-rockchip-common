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

public class RPCWrapper {

    private final String TAG = "AMTL";
    private final String MODULE = "RPCWrapper";

    /* Load RPC JNI library */
    static {
        System.loadLibrary("rpc_jni");
    }

    private native String SendRPCCommand(String cmd);
    private native void GenerateCoredump();


    public String sendRPCCall(String cmd) {
        AlogMarker.tAB("RPCWrapper.sendRPCCall", "0");
        cmd = cmd.replace("\r\n", "");
        Log.d(TAG, MODULE + " " + cmd);
        AlogMarker.tAE("RPCWrapper.sendRPCCall", "0");
        return SendRPCCommand(cmd);
    }

    public void generateModemCoredump() {
        AlogMarker.tAB("RPCWrapper.generateModemCoredump", "0");
        Log.d(TAG, MODULE + ": coredump generation");
        GenerateCoredump();
        AlogMarker.tAE("RPCWrapper.generateModemCoredump", "0");
    }
}
