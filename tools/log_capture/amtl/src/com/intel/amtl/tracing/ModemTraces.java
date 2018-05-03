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
 * Author: Nicolae Natea <nicolaex.natea@intel.com>
 */

package com.intel.amtl.tracing;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.FileObserver;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.widget.Switch;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.gui.ActionMenu;
import com.intel.amtl.gui.ConfigApplyFrag;
import com.intel.amtl.helper.FileOperations;
import com.intel.amtl.helper.LogManager;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.ExpertConfig;
import com.intel.amtl.models.config.ModemConf;
import com.intel.amtl.models.ConfigManager;
import com.intel.amtl.modem.controller.ModemController;
import com.intel.amtl.mts.MtsManager;
import com.intel.amtl.StoredSettings;
import com.intel.amtl.R;

import java.io.IOException;
import java.util.ArrayList;


public class ModemTraces implements GeneralTracing {
    Process logcatProc = null;
    private final String TAG = "AMTL";
    private final String MODULE = "ModemTraces";

    private final String CONFSETUP_TAG = "AMTL_modem_configuration_setup";
    private final int CONFSETUP_TARGETFRAG = 0;

    private Runtime rtm = java.lang.Runtime.getRuntime();

    private boolean running = false;
    private String modemName = "";

    private String lastStatus = "";

    private Fragment parent = null;
    private static StoredSettings sSettings = null;

    public ModemTraces(Fragment parent, String modemName) {

        AlogMarker.tAB("ModemTraces.ModemTraces", "0");
        this.parent = parent;

        if (sSettings == null) {
            sSettings = new StoredSettings(AMTLApplication.getContext());
        }
        this.modemName = modemName;
        AlogMarker.tAE("ModemTraces.ModemTraces", "0");
    }

    public boolean start() {
        AlogMarker.tAB("ModemTraces.start", "0");

        setRunning(startModemTracing());
        AlogMarker.tAE("ModemTraces.start", "0");

        return running;
    }

    public void stop() {
        AlogMarker.tAB("ModemTraces.stop", "0");
        if (isRunning()) {
            setRunning(false);
            stopModemTracing();
        }
        AlogMarker.tAE("ModemTraces.stop", "0");
    }

    public void setState(boolean state) {
        // nothing to do
    }

    public boolean restart() {
        return start();
    }

    public boolean isUpdated(View view) {
        ModemConf modemConfToApply = ActionMenu.getModemConfiguration();
        int nextConfig = -1;
        int runningConfig = sSettings.getStoredIntValue("index" + modemName, -1);

        if (modemConfToApply != null) {
            nextConfig = modemConfToApply.getIndex();
        }

        return (runningConfig != nextConfig);
    }

    public void cleanTemp() {
        AlogMarker.tAB("ModemTraces.cleanTemp", "0");
        if (MtsManager.getMtsState().equals("running")
                && MtsManager.getMtsOutputType().equals("f")) {
            MtsManager.stopServices();
            FileOperations.removeFiles(sSettings.getBPLoggingPath() + "/",
                    FileOperations.BP_LOG_FILE_NAME_MATCH);
            MtsManager.startService("persistent");
        } else {
            FileOperations.removeFiles(sSettings.getBPLoggingPath() + "/",
                    FileOperations.BP_LOG_FILE_NAME_MATCH);
        }

        AlogMarker.tAE("ModemTraces.cleanTemp", "0");
    }

    public void saveTemp(String path) {
        AlogMarker.tAB("ModemTraces.saveTemp", "0");

        LogManager snaplog = new LogManager(path, "/mnt/sdcard/"
                + sSettings.getRelativeStorePath() + "/", sSettings.getBPLoggingPath() + "/");
        if (snaplog == null) {
            AlogMarker.tAE("ModemTraces.saveTemp", "0");
            return;
        }

        AlogMarker.tAE("ModemTraces.saveTemp", "0");
        snaplog.makeBackup(path, false);
    }

    private boolean startModemTracing() {
        AlogMarker.tAB("ModemTraces.saveModemTracing", "0");
        ModemConf modemConfToApply = ActionMenu.getModemConfiguration();
        boolean atProxyToStart = ActionMenu.getAtProxyStatus();
        if (modemConfToApply == null) {
            Log.e(TAG, MODULE + ": Could not start modem logging as no configuration set up");
            lastStatus = "No modem configuration selected.";
            sSettings.setKV("index" + modemName, -1);
            AlogMarker.tAE("ModemTraces.saveModemTracing", "0");
            return false;
        }

        boolean expEnabled = ExpertConfig.isExpertModeEnabled(modemName);
        SharedPreferences prefs = AMTLApplication.getContext()
                .getSharedPreferences("AMTLPrefsData", Context.MODE_PRIVATE);

        if (!expEnabled && prefs.getInt("index" + modemName, -2) == modemConfToApply.getIndex()) {
            ConfigManager.applyAtProxyConfig(modemConfToApply, atProxyToStart);
            Log.d(TAG, MODULE + ": configuration selected is the same as the current one");
            lastStatus = "Configuration has not changed";
            AlogMarker.tAE("ModemTraces.saveModemTracing", "0");
            return false;
        }

        sSettings.setKV("index" + modemName, modemConfToApply.getIndex());
        ConfigApplyFrag progressFrag = ConfigApplyFrag.newInstance(CONFSETUP_TAG,
                CONFSETUP_TARGETFRAG);
        progressFrag.launch(modemConfToApply, atProxyToStart, this.parent,
                AMTLApplication.getActivity().getFragmentManager());

        lastStatus = "Modem tracing started";
        AlogMarker.tAE("ModemTraces.saveModemTracing", "0");
        return (modemConfToApply != null && modemConfToApply.getIndex() != -1);
    }

    private void stopModemTracing() {
        AlogMarker.tAB("ModemTraces.stopModemTracing", "0");
        ModemController modemCtrl;
        sSettings.setKV("index" + modemName, -1);

        if (!isRunning()) {
            AlogMarker.tAE("ModemTraces.stopModemTracing", "0");
            return;
        }

        try {
            Log.d(TAG, MODULE + ": Stopping trace BP");
            modemCtrl = ModemController.getInstance();

            ModemConf modConfToApply = modemCtrl.getNoLoggingConf();
            ConfigApplyFrag progressFrag = ConfigApplyFrag.newInstance(CONFSETUP_TAG,
                    CONFSETUP_TARGETFRAG);
            progressFrag.launch(modConfToApply, ActionMenu.getAtProxyStatus(), this.parent,
                    AMTLApplication.getActivity().getFragmentManager());
            ExpertConfig.setExpertMode(modemName, false);
        } catch (ModemControlException ex) {
            Log.e(TAG, MODULE + " " + ex);
        } finally {
            modemCtrl = null;
        }
        AlogMarker.tAE("ModemTraces.stopModemTracing", "0");
    }

    public boolean isRunning() {
        AlogMarker.tAB("ModemTraces.isRunning", "0");
        AlogMarker.tAE("ModemTraces.isRunning", "0");
        return sSettings.getStoredValue("modemTraces_enabled" + modemName);
    }

    public void updateConfiguration() {
        AlogMarker.tAB("ModemTraces.updateConfiguration", "0");
        AlogMarker.tAE("ModemTraces.updateConfiguration", "0");
    }

    public int getViewID() {
        AlogMarker.tAB("ModemTraces.getViewID", "0");
        AlogMarker.tAE("ModemTraces.getViewID", "0");
        return R.layout.generalsetupfraglayout;
    }

    public void attachReferences(View view) {
        AlogMarker.tAB("ModemTraces.attachReferences", "0");
        AlogMarker.tAE("ModemTraces.attachReferences", "0");
    }

    public void attachListeners() {
        AlogMarker.tAB("ModemTraces.attachListeners", "0");
        AlogMarker.tAE("ModemTraces.attachListeners", "0");
    }

    private void setRunning(boolean enabled) {
        AlogMarker.tAB("ModemTraces.setRunning", "0");
        running = enabled;
        AlogMarker.tAE("ModemTraces.setRunning", "0");
    }

    public String getLastStatus() {
        AlogMarker.tAB("ModemTraces.getLastStatus", "0");
        AlogMarker.tAE("ModemTraces.getLastStatus", "0");
        return lastStatus;
    }

    public String getTracerName() {
        AlogMarker.tAB("ModemTraces.getTracerName", "0");
        AlogMarker.tAE("ModemTraces.getTracerName", "0");
        return "Modem Traces";
    }
}
