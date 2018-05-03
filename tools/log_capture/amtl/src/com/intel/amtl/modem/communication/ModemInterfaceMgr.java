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

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.log.AlogMarker;

import java.io.Closeable;
import java.io.IOException;
import java.io.RandomAccessFile;

public class ModemInterfaceMgr implements Closeable {

    private final String TAG = "AMTL";
    private final String MODULE = "ModemInterfaceMgr";
    private RandomAccessFile file = null;
    private ModemInterface mdmInterface = null;
    private RPCWrapper rpcWrapper;

    public ModemInterfaceMgr() throws ModemControlException {
        AlogMarker.tAB("ModemInterfaceMgr.ModemInterfaceMgr", "0");

        try {
            if (this.getModemInterface() != null) {
                if (isModemIfTty()) {
                    this.mdmInterface = new ModemInterface(this.getModemInterface());
                    this.mdmInterface.openInterface();
                }
                this.file = new RandomAccessFile(this.getModemInterface(), "rw");
            } else {
                rpcWrapper = new RPCWrapper();
            }
        } catch (ExceptionInInitializerError ex) {
            throw new ModemControlException("libamtl_jni library was not found " + ex);
        } catch (IOException ex) {
            throw new ModemControlException("Error while opening modem interface " + ex);
        } catch (IllegalArgumentException ex) {
            throw new ModemControlException("Error while opening modem interface" + ex);
        }
        AlogMarker.tAE("ModemInterfaceMgr.ModemInterfaceMgr", "0");
    }

    private boolean isModemIfTty() {
        return this.getModemInterface().contains("tty");
    }

    public String getModemInterface() {
        return AMTLApplication.getModemInterface();
    }

    public String sendCommand(String cmd) throws ModemControlException {
        AlogMarker.tAB("ModemInterfaceMgr.sendCommand", "0");
        String ret = "NOK";

        if (this.getModemInterface() != null) {
            writeToModemControl(cmd);

            ret = "";
            do {
                //Response may return in two lines.
                ret += readFromModemControl();
            } while (!ret.contains("OK") && !ret.contains("ERROR")
                    && !cmd.equals("at+xsystrace=pn#\r\n"));
        } else {
            if (rpcWrapper != null) {
                ret = rpcWrapper.sendRPCCall(cmd);
                Log.i(TAG, MODULE + " : response from modem " + ret);
            }
        }
        if (ret.contains("ERROR")) {
            throw new ModemControlException("Modem has answered " + ret
                    + " to the AT command sent " + cmd);
        }
        AlogMarker.tAE("ModemInterfaceMgr.sendCommand", "0");
        return ret;
    }

    public void generateModemCoredump() throws ModemControlException {
        AlogMarker.tAB("ModemInterfaceMgr.generateCoredump", "0");
        if (this.getModemInterface() != null) {
            writeToModemControl("AT+XLOG=4\r\n");
        } else {
            rpcWrapper.generateModemCoredump();
        }
        AlogMarker.tAE("ModemInterfaceMgr.generateCoredump", "0");
    }

    private void writeToModemControl(String command) throws ModemControlException {
        AlogMarker.tAB("ModemInterfaceMgr.writeToModemControl", "0");

        try {
            this.file.writeBytes(command);
            Log.i(TAG, MODULE + ": sending to modem " + command);
        } catch (IOException ex) {
            throw new ModemControlException("Unable to send to command to the modem. " + ex);
        }
        AlogMarker.tAE("ModemInterfaceMgr.writeToModemControl", "0");
    }

    private String readFromModemControl() throws ModemControlException {
        AlogMarker.tAB("ModemInterfaceMgr.readFromModemControl", "0");

        String response = "";
        byte[] responseBuffer = new byte[1024];

        try {
            int readCount = this.file.read(responseBuffer);
            if (readCount >= 0) {
                response = new String(responseBuffer, 0, readCount);
                Log.i(TAG, MODULE + " : response from modem " + response);
            } else {
                throw new ModemControlException("Unable to read response from the modem.");
            }
        } catch (IOException ex) {
            throw new ModemControlException("Unable to read response from the modem.");
        }
        AlogMarker.tAE("ModemInterfaceMgr.readFromModemControl", "0");
        return response;
    }

    public void close() {
        AlogMarker.tAB("ModemInterfaceMgr.close", "0");
        if (this.file != null) {
            try {
                this.file.close();
                this.file = null;
            } catch (IOException ex) {
                Log.e(TAG, MODULE + ex.toString());
            } finally {
                if (this.mdmInterface != null) {
                    this.mdmInterface.closeInterface();
                    this.mdmInterface = null;
                }
            }
        }
        AlogMarker.tAE("ModemInterfaceMgr.close", "0");
    }

    public void closeModemInterface() {
        AlogMarker.tAB("ModemInterfaceMgr.closeModemInterface", "0");
        this.close();
        AlogMarker.tAE("ModemInterfaceMgr.closeModemInterface", "0");
    }
}
