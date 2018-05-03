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

package com.intel.amtl.modem.controller;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.util.Log;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.Master;
import com.intel.amtl.models.config.ModemConf;
import com.intel.amtl.models.config.Master;
import com.intel.amtl.models.config.ModemConf;
import com.intel.amtl.modem.command.CommandParser;
import com.intel.amtl.modem.communication.ModemInterfaceMgr;
import com.intel.amtl.modem.output_config.IoctlConfig;
import com.intel.amtl.modem.output_config.LogOutputConfig;
import com.intel.amtl.modem.output_config.XsioConfig;
import com.intel.internal.telephony.DebugInfoLog;
import com.intel.internal.telephony.DebugInfoType;
import com.intel.internal.telephony.ModemClientException;
import com.intel.internal.telephony.ModemEventListener;
import com.intel.internal.telephony.ModemStatus;
import com.intel.internal.telephony.ModemStatusManager;

import java.io.IOException;
import java.lang.Object;
import java.util.ArrayList;

public abstract class ModemController implements ModemEventListener {

    private final String TAG = "AMTL";
    private final String MODULE = "ModemController";

    private ModemStatusManager modemStatusManager;
    private static ModemStatus currentModemStatus = ModemStatus.NONE;
    private static boolean modemAcquired = false;
    private ArrayList<Master> masterArray;
    private boolean firstAcquire = true;
    protected static ModemInterfaceMgr modIfMgr;
    private LogOutputConfig logOutputConf;
    private static ModemController mdmCtrl;

    private static final String PERSIST_PROUTE = "persist.sys.mmgr"
            + AMTLApplication.getModemConnectionId() + ".proute";

    public abstract boolean queryTraceState() throws ModemControlException;
    public abstract String switchOffTrace() throws ModemControlException;
    public abstract void switchTrace(ModemConf mdmconf) throws ModemControlException;
    public abstract String checkAtTraceState() throws ModemControlException;
    public abstract ModemConf getNoLoggingConf();

    public ModemController() throws ModemControlException {
        AlogMarker.tAB("ModemController.ModemController", "0");
        logOutputConf = (AMTLApplication.getXsioDefined()) ? new XsioConfig() : new IoctlConfig();
        try {
            this.modemStatusManager = ModemStatusManager
                    .getInstance(AMTLApplication.getContext(),
                     AMTLApplication.getModemConnectionId());
        } catch (InstantiationException ex) {
            throw new ModemControlException("Cannot instantiate Modem Status Manager");
        }
        AlogMarker.tAE("ModemController.ModemController", "0");
    }

    public static synchronized ModemController getInstance() throws ModemControlException {
        AlogMarker.tAB("ModemController.getInstance", "0");
        if (null == mdmCtrl) {
            mdmCtrl = (AMTLApplication.getTraceLegacy())
                    ? new TraceLegacyController() : (AMTLApplication.getIsAliasUsed())
                    ? new AliasController() : new OctController();
        }
        AlogMarker.tAE("ModemController.getInstance", "0");
        return mdmCtrl;
    }

    public void connectToModem() throws ModemControlException {
        AlogMarker.tAB("ModemController.connectToModem", "0");
        if (this.modemStatusManager != null) {
            try {
                Log.d(TAG, MODULE + ": Subscribing to Modem Status Manager");
                this.modemStatusManager.subscribeToEvent(this, ModemStatus.ALL);
            } catch (ModemClientException ex) {
                throw new ModemControlException("Cannot subscribe to Modem Status Manager " + ex);
            }
            try {
                Log.d(TAG, MODULE + ": Connecting to Modem Status Manager");
                this.modemStatusManager.connect("AMTL");
            } catch (ModemClientException ex) {
                throw new ModemControlException("Cannot connect to Modem Status Manager " + ex);
            }
            try {
                Log.d(TAG, MODULE + ": Acquiring modem resource");
                this.modemStatusManager.acquireModem();
                this.modemAcquired = true;
                this.firstAcquire = false;
            } catch (ModemClientException ex) {
                throw new ModemControlException("Cannot acquire modem resource " + ex);
            }
        }
        AlogMarker.tAE("ModemController.connectToModem", "0");
    }

    public String getModemStatus() {
        AlogMarker.tAB("ModemController.getModemStatus", "0");
        switch (currentModemStatus) {
            case UP:
                AlogMarker.tAE("ModemController.getModemStatus", "0");
                return "UP";
            case DOWN:
                AlogMarker.tAE("ModemController.getModemStatus", "0");
                return "DOWN";
            case DEAD:
                AlogMarker.tAE("ModemController.getModemStatus", "0");
                return "DEAD";
            case NONE:
            default:
                AlogMarker.tAE("ModemController.getModemStatus", "0");
                return "NONE";
        }
    }

    // restart the modem by asking for a cold reset
    public void restartModem() throws ModemControlException {
        AlogMarker.tAB("ModemController.restartModem", "0");
        if (!AMTLApplication.getModemRestart()) {
            if (this.isModemUp()) {
                SystemClock.sleep(2000);
                sendMessage("UP");
            }
        } else {
            if (this.modemStatusManager != null) {
                try {
                    Log.d(TAG, MODULE + ": Asking for modem restart");
                    this.modemStatusManager.updateModem();
                    SystemClock.sleep(2000);
                } catch (ModemClientException ex) {
                    throw new ModemControlException("Cannot restart modem");
                }
            }
        }
        AlogMarker.tAE("ModemController.restartModem", "0");
    }

    public String sendCommand(String command) throws ModemControlException {
        AlogMarker.tAB("ModemController.sendCommand", "0");
        String ret = "NOK";

        if (command != null) {
            if (!command.equals("")) {
                if (!this.isModemUp()) {
                    throw new ModemControlException("Cannot send command, "
                            + "modem is not ready: status = " + this.currentModemStatus);
                }
                if (modIfMgr != null) {
                    ret = modIfMgr.sendCommand(command);
                } else {
                    throw new ModemControlException("Cannot send command, "
                            + "no interface available to send command");
                }
            }
        }
        AlogMarker.tAE("ModemController.sendCommand", "0");
        return ret;
    }

    public String flush(ModemConf mdmConf) throws ModemControlException {
        AlogMarker.tAB("ModemController.flush", "0");
        AlogMarker.tAE("ModemController.flush", "0");
        return sendCommand(mdmConf.getFlCmd());
    }

    public String confTraceAndModemInfo(ModemConf mdmConf) throws ModemControlException {
        AlogMarker.tAB("ModemController.confTraceAndModemInfo", "0");
        AlogMarker.tAE("ModemController.confTraceAndModemInfo", "0");
        return logOutputConf.confTraceAndModemInfo(mdmConf, mdmCtrl);
    }

    public String checkAtXsioState() throws ModemControlException {
        AlogMarker.tAB("ModemController.checkAtXsioState", "0");
        AlogMarker.tAE("ModemController.checkAtXsioState", "0");
        return logOutputConf.checkAtXsioState(mdmCtrl);
    }

    public String checkAtXsystraceState() throws ModemControlException {
        AlogMarker.tAB("ModemController.checkAtXsystraceState", "0");
        AlogMarker.tAE("ModemController.checkAtXsystraceState", "0");
        return sendCommand("at+xsystrace=10\r\n");
    }

    public ArrayList<Master> checkAtXsystraceState(ArrayList<Master> masterList)
            throws ModemControlException {
        AlogMarker.tAB("ModemController.checkAtXsystraceState", "0");
        AlogMarker.tAE("ModemController.checkAtXsystraceState", "0");
        return CommandParser.parseXsystraceResponse(sendCommand("at+xsystrace=10\r\n"),
                masterList);
    }

    public String checkOct() throws ModemControlException {
        AlogMarker.tAB("ModemController.checkOct", "0");
        AlogMarker.tAE("ModemController.checkOct", "0");
        return CommandParser.parseOct(sendCommand("at+xsystrace=11\r\n"));
    }

    public String checkProfileName() throws ModemControlException {
        AlogMarker.tAB("ModemController.checkProfileName", "0");
        AlogMarker.tAE("ModemController.checkProfileName", "0");
        return CommandParser.parseProfileName(sendCommand("at+xsystrace=pn#\r\n"));
    }

    public String getAtXsioState() throws ModemControlException {
        AlogMarker.tAB("ModemController.getAtXsioState", "0");
        AlogMarker.tAE("ModemController.getAtXsioState", "0");
        return logOutputConf.getAtXsioState(mdmCtrl);
    }

    public void generateModemCoreDump() throws ModemControlException {
        AlogMarker.tAB("ModemController.generateModemCoreDump", "0");

        if (!this.isModemUp()) {
            throw new ModemControlException("Cannot generate modem coredump, "
                    + "modem is not ready: status = " + this.currentModemStatus);
        }
        if (modIfMgr != null) {
            modIfMgr.generateModemCoredump();
        } else {
            throw new ModemControlException("Cannot generate modem coredump, "
                    + "no interface available to send command");
        }
        AlogMarker.tAE("ModemController.generateModemCoreDump", "0");
    }

    public void releaseResource() {
        AlogMarker.tAB("ModemController.releaseResource", "0");
        if (this.modemStatusManager != null) {
            try {
                if (this.modemAcquired) {
                    this.modemStatusManager.releaseModem();
                    this.modemAcquired = false;
                }
            } catch (ModemClientException ex) {
                Log.e(TAG, MODULE + ": Cannot release modem resource");
            }
        }
        AlogMarker.tAE("ModemController.releaseResource", "0");
    }

    public boolean isModemAcquired() {
        AlogMarker.tAB("ModemController.isModemAcquired", "0");
        AlogMarker.tAE("ModemController.isModemAcquired", "0");
        return this.modemAcquired;
    }

    public void acquireResource() throws ModemControlException  {
        AlogMarker.tAB("ModemController.acquireResource", "0");
        try {
            if (this.modemStatusManager != null) {
                if (!this.modemAcquired && !this.firstAcquire) {
                    this.modemStatusManager.acquireModem();
                    this.modemAcquired = true;
                }
            }
        } catch (ModemClientException ex) {
            throw new ModemControlException("Could not acquire modem" + ex);
        }
        AlogMarker.tAE("ModemController.acquireResource", "0");
    }

    protected void disconnectModemStatusManager() {
        AlogMarker.tAB("ModemController.disconnectModemStatusManager", "0");
        if (modemStatusManager != null) {
            releaseResource();
            modemStatusManager.disconnect();
            modemStatusManager = null;
        }
        AlogMarker.tAE("ModemController.disconnectModemStatusManager", "0");
    }

    public void cleanBeforeExit() {
        AlogMarker.tAB("ModemController.cleanBeforeExit", "0");
        this.closeModemInterface();
        disconnectModemStatusManager();
        this.mdmCtrl = null;
        AlogMarker.tAE("ModemController.cleanBeforeExit", "0");
    }

    public boolean isModemUp() {
        AlogMarker.tAB("ModemController.isModemUp", "0");
        AlogMarker.tAE("ModemController.isModemUp", "0");
        return (currentModemStatus == ModemStatus.UP) ? true : false;
    }

    public String checkPRoute(String cmd) {
        String ret = "";
        if (cmd != null) {
            int indexPRoute = cmd.indexOf("proute: ");
            if (indexPRoute >= 0) {
                String sub = cmd.substring(indexPRoute + 8);
                ret = sub.substring(0, sub.indexOf("\r\n"));
            }
        }
        return ret;
    }

    public void setProuteInfo(String sysTrace, String xsio) throws ModemControlException {
        String pRoute = checkPRoute(sysTrace);
        if ("Oct".equals(pRoute)) {
            if (xsio != null) {
                String xSioState = getAtXsioState();
                if (xSioState == null) {
                    return;
                }
                String var = "Variant=";
                String variant = var.concat(xsio);
                int indexOfVariant = xSioState.indexOf(variant);
                String subStr = xSioState.substring(indexOfVariant,
                        xSioState.indexOf("\r\n", indexOfVariant));
                int routeIndex = subStr.indexOf("OCT= ");
                String route = subStr.substring(routeIndex + 5,
                        subStr.indexOf(";", routeIndex + 5));
                Log.d(TAG, MODULE + " route info: " + route);
                if ("USB1".equalsIgnoreCase(route)) {
                    SystemProperties.set(PERSIST_PROUTE, "external-USB");
                } else {
                    SystemProperties.set(PERSIST_PROUTE, "internal");
                }
            }
        } else if ("Mipi2".equalsIgnoreCase(pRoute)) {
            SystemProperties.set(PERSIST_PROUTE, "external-MIPI");
        } else {
            SystemProperties.set(PERSIST_PROUTE, "internal");
        }
    }

    public void notifyDebug(String issue, boolean ap, boolean bp) {
        AlogMarker.tAB("MSMController.notifyDebug", "0");
        String[] cause = {"TFT_USER_TRIGGERED_REPORTING", issue};
        int logToEvacuate = DebugInfoLog.ATTACH_NO_LOG;
        if (ap && !bp) {
            logToEvacuate = DebugInfoLog.ATTACH_AP_LOG;
        } else if (!ap && bp) {
            logToEvacuate = DebugInfoLog.ATTACH_BP_LOG;
        } else if (ap && bp) {
            logToEvacuate = DebugInfoLog.ATTACH_AP_AND_BP_LOG;
        }
        try {
            Log.d(TAG, MODULE + " notifying crashtool " + issue + " " + logToEvacuate);
            this.modemStatusManager.notifyDebugInfo(cause, DebugInfoType.INFO, logToEvacuate);
        } catch (ModemClientException e) {
            Log.e(TAG, MODULE + ": Cannot notify debug info " + e);
        }
        AlogMarker.tAE("MSMController.notifyDebug", "0");
    }

    @Override
    public void onModemUp() {
        AlogMarker.tAB("ModemController.onModemUp", "0");
        this.currentModemStatus = ModemStatus.UP;
        Log.d(TAG, MODULE + ": Modem is UP");
        try {
            AMTLApplication.setCloseTtyEnable(false);
            openModemInterface();
            sendMessage("UP");
        } catch (ModemControlException ex) {
            Log.e(TAG, MODULE + ex);
        }
        AlogMarker.tAE("ModemController.onModemUp", "0");
    }

    @Override
    public void onModemDown() {
        AlogMarker.tAB("ModemController.onModemDown", "0");
        this.currentModemStatus = ModemStatus.DOWN;
        Log.d(TAG, MODULE + ": Modem is DOWN");
        sendMessage("DOWN");
        closeModemInterface();
        AlogMarker.tAE("ModemController.onModemDown", "0");
    }

    @Override
    public void onModemDead() {
        AlogMarker.tAB("ModemController.onModemDead", "0");
        this.onModemDown();
        this.currentModemStatus = ModemStatus.DEAD;
        Log.d(TAG, MODULE + ": Modem is DEAD");
        AlogMarker.tAE("ModemController.onModemDead", "0");
    }

    public void sendFlushCmd(ModemConf curModConf) throws ModemControlException {
        // if flush command available for this configuration, let s use it.
        if (!curModConf.getFlCmd().equalsIgnoreCase("")) {
            Log.d(TAG, MODULE + ": Config has flush_cmd defined.");
            flush(curModConf);
            // give time to the modem to sync - 1 second
            SystemClock.sleep(1000);
        } else {
            // fall back - check if a default flush cmd is set
            Log.d(TAG, MODULE + ": Fall back - check default_flush_cmd");
            SharedPreferences prefs = AMTLApplication.getContext()
                    .getSharedPreferences("AMTLPrefsData", Context.MODE_PRIVATE);
            if (prefs != null) {
                String flCmd = prefs.getString("default_flush_cmd", "");
                if (!flCmd.equalsIgnoreCase("")) {
                    sendCommand(flCmd + "\r\n");
                    // give time to the modem to sync - 1 second
                    SystemClock.sleep(1000);
                }
            }
        }
    }

    public void closeModemInterface() {
        AlogMarker.tAB("ModemController.closeModemInterface", "0");
        if (this.modIfMgr != null) {
            this.modIfMgr.closeModemInterface();
            this.modIfMgr = null;
        }
        AlogMarker.tAE("ModemController.closeModemInterface", "0");
    }

    public void openModemInterface() throws ModemControlException {
        AlogMarker.tAB("ModemController.openModemInterface", "0");
        if (this.modIfMgr == null) {
            this.modIfMgr = new ModemInterfaceMgr();
        }
        AlogMarker.tAE("ModemController.closeModemInterface", "0");
    }

    public void sendMessage(String msg) {
        AlogMarker.tAB("ModemController.sendMessage", "0");
        Intent intent = new Intent("modem-event");
        intent.putExtra("message", msg);
        AMTLApplication.getContext().sendBroadcastAsUser(intent,
                new UserHandle(UserHandle.USER_CURRENT));
        AlogMarker.tAE("ModemController.sendMessage", "0");
    }

    public String getOctDriverPath() {
        AlogMarker.tAB("IoctlConfig.getOctDriverPath", "0");
        AlogMarker.tAE("IoctlConfig.getOctDriverPath", "0");
        return logOutputConf.getOctDriverPath();
    }
}
