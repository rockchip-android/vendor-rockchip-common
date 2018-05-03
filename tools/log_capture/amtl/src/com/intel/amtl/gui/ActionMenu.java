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

package com.intel.amtl.gui;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.PorterDuff.Mode;
import android.graphics.drawable.Drawable;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.Toast;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.helper.FileOperations;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.ModemConf;
import com.intel.amtl.modem.controller.ModemController;
import com.intel.amtl.StoredSettings;
import com.intel.amtl.tracing.GeneralTracing;
import com.intel.amtl.tracing.TraceList;
import com.intel.amtl.R;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class ActionMenu implements OnClickListener, GeneralTracing {

    private final String TAG = "AMTL";
    private final String MODULE = "ActionMenu";

    static TraceList tracers = new TraceList();
    Toast toast = null;

    private static ActionMenu sInstance = null;

    private ImageButton buttonStart = null;
    private ImageButton buttonStop = null;
    private ImageButton buttonSave = null;
    private ImageButton buttonClear = null;

    private String outputFile = getRelativeStorePath();
    private static StoredSettings sSettings = new StoredSettings(AMTLApplication.getContext());
    private boolean outputSDCard = false;
    private View view = null;

    private boolean running = false;

    public ActionMenu() {
        AlogMarker.tAB("ActionMenu.ActionMenu", "0");
        switchOutput();
        sInstance = this;
        AlogMarker.tAE("ActionMenu.ActionMenu", "0");
    }

    public static ActionMenu getInstance() {
        return sInstance;
    }

    public static void unsetInstance() {
        sInstance = null;
    }

    public boolean start() {
        AlogMarker.tAB("ActionMenu.start", "0");
        int issues = 0;
        TraceList tl;

        Log.d(TAG, MODULE + ": Starting traces");
        if (isRunning()) {
            return restart();
        }

        tl = AMTLTabLayout.getActiveTraces();

        for (TraceList.Tracer t : tl.getTracers()) {
            GeneralTracing gt = t.getLogger();
            if (!gt.start()) {
                issues++;
                tracers.unRegisterTracer(t);
            }
            else {
                tracers.registerTracer(t);
            }
        }

        setRunning(tracers.getTracers().size() > 0);

        AlogMarker.tAE("ActionMenu.start", "0");
        return true;
    }

    public void stop() {
        AlogMarker.tAB("ActionMenu.stop", "0");
        Log.d(TAG, MODULE + ": Stopping traces");


        for (Iterator<TraceList.Tracer> it = tracers.getTracers().iterator(); it.hasNext();) {
            TraceList.Tracer t = it.next();
            GeneralTracing gt = t.getLogger();
            gt.stop();
            it.remove();
        }
        setRunning(false);
        AlogMarker.tAE("ActionMenu.stop", "0");
    }

    public void setState(boolean state) {
        if (this.buttonStart != null) {
            this.buttonStart.setEnabled(state);
            this.buttonStop.setEnabled(state);
            this.buttonStart.setImageDrawable(
                    getBackgroundImage(R.drawable.start_button, state));
            this.buttonStop.setImageDrawable(
                    getBackgroundImage(R.drawable.stop_button, state));
        }
    }

    private Drawable getBackgroundImage(int id, boolean enabled) {
        if (AMTLApplication.getActivity() == null) {
            return null;
        }

        Drawable original = AMTLApplication.getActivity().getResources().getDrawable(id);

        if (enabled) {
            return original;
        }

        Drawable grayed = original.mutate();
        grayed.setColorFilter(Color.GRAY, Mode.SRC_IN);

        return grayed;
    }

    public boolean restart() {
        AlogMarker.tAB("ActionMenu.restart", "0");
        TraceList activated = AMTLTabLayout.getActiveTraces();

        for (Iterator<TraceList.Tracer> it = tracers.getTracers().iterator(); it.hasNext();) {
            TraceList.Tracer t = it.next();
            if (activated.findTracer(t) == null) {
                t.getLogger().stop();
                it.remove();
            }
        }

        tracers.mergeTracers(activated);

        for (Iterator<TraceList.Tracer> it = tracers.getTracers().iterator(); it.hasNext();) {
            TraceList.Tracer t = it.next();
            boolean startedOk = true;
            if (this.view != null && t.getLogger().isUpdated(this.view)) {
                startedOk = t.getLogger().restart();
            }

            if (!startedOk) {
                it.remove();
            }
        }

        setRunning(!tracers.getTracers().isEmpty());
        AlogMarker.tAE("ActionMenu.restart", "0");
        return running;
    }

    public boolean isUpdated(View view) {
        return false;
    }

    public void cleanTemp() {
        AlogMarker.tAB("ActionMenu.cleanTemp", "0");

        Runnable ok = new Runnable() {
            @Override
            public void run() {
                for (Iterator<TraceList.Tracer> it = tracers.getTracers().iterator();
                        it.hasNext();) {
                    TraceList.Tracer t = it.next();
                    GeneralTracing gt = t.getLogger();
                    gt.cleanTemp();
                }
                Toast toast = Toast.makeText(AMTLApplication.getActivity(),
                        "Clear temporary files", Toast.LENGTH_SHORT);
                toast.show();
            }
        };

        Runnable cancel = new Runnable() {
            @Override
            public void run() {
                //nothing to do
            }
        };

        UIHelper.cleanPopupDialog(AMTLApplication.getActivity(), "Clean temp files",
                "Are you sure you want to delete these files", ok, cancel);
        AlogMarker.tAE("ActionMenu.cleanTemp", "0");
    }

    public void saveTemp(String path) {
        AlogMarker.tAB("ActionMenu.saveTemp", "0");

        for (TraceList.Tracer t : tracers.getTracers()) {
            GeneralTracing gt = t.getLogger();
            running = gt.isRunning();
            gt.saveTemp(path);
            if (running) {
                gt.start();
            }
        }
        AlogMarker.tAE("ActionMenu.saveTemp", "0");
    }

    private String save() {
        AlogMarker.tAB("ActionMenu.save", "0");
        String path = null;

        Runnable ok = new Runnable() {
            @Override
            public void run() {
                String path = PreferenceManager.getDefaultSharedPreferences(
                        AMTLApplication.getActivity()).getString(
                        AMTLApplication.getActivity().getString(
                        R.string.settings_user_save_path_key), getRelativeStorePath() + "/");
                boolean pathExists = false;

                try {
                    pathExists = FileOperations.createPath(path);
                } catch (IOException e) {
                    Log.e(TAG, MODULE + ": Error while create saving path: " + e);
                }

                if (pathExists) {
                    Toast toast = Toast.makeText(AMTLApplication.getActivity(),
                            "Saving to " + path, Toast.LENGTH_SHORT);
                    toast.show();
                    Log.d(TAG, MODULE + ": User action for saving logs to: " + path);
                    saveTemp(path);
                } else {
                    Toast toast = Toast.makeText(AMTLApplication.getActivity(), "Not saved! \n"
                            + "Path could not be created.", Toast.LENGTH_SHORT);
                    Log.e(TAG, MODULE + ": Not saved! \nPath could not be created." + path);
                    toast.show();
                }
            }
        };

        Runnable cancel = new Runnable() {
            @Override
            public void run() {
                //nothing to do
            }
        };

        path = PreferenceManager.getDefaultSharedPreferences(AMTLApplication.getActivity()).
                getString(AMTLApplication.getActivity().getString(R.string.settings_save_path_key),
                getRelativeStorePath());
        path = FileOperations.getTimeStampedPath(path, "logs_");

        UIHelper.savePopupDialog(AMTLApplication.getActivity(), "Save active logs",
                "Please select the path where the current logs should be stored!", path,
                AMTLApplication.getActivity(), ok, cancel);

        AlogMarker.tAE("ActionMenu.save", "0");
        return path;
    }

    public boolean isRunning() {
        AlogMarker.tAB("ActionMenu.isRunning", "0");
        AlogMarker.tAE("ActionMenu.isRunning", "0");
        return recheckTracingState();
    }

    private void updateOutputPath(boolean toSDCard) {
        AlogMarker.tAB("ActionMenu.updateOutputPath", "0");
        String relativePath = getRelativeStorePath();

        if (toSDCard) {
            outputFile = FileOperations.getSDStoragePath();
        } else {
            outputFile = getRelativeStorePath();
        }

        if (relativePath != "") {
            outputFile += "/" + relativePath;
        }
        AlogMarker.tAE("ActionMenu.updateOutputPath", "0");
    }

    private void refreshOutputPath() {
        AlogMarker.tAB("ActionMenu.refreshOutputPath", "0");
        outputSDCard = !outputSDCard;
        switchOutput();
        AlogMarker.tAE("ActionMenu.refreshOutputPath", "0");
    }

    public boolean switchOutput() {
        AlogMarker.tAB("ActionMenu.switchOutput", "0");
        outputSDCard = !outputSDCard;

        if (outputSDCard) {
            if (!FileOperations.isSdCardAvailable()) {
                outputSDCard = false;
                AlogMarker.tAE("ActionMenu.switchOutput", "0");
                return false;
            }
        }

        updateOutputPath(outputSDCard);
        AlogMarker.tAE("ActionMenu.switchOutput", "0");

        return true;
    }

    private void setRunning(boolean enabled) {
        AlogMarker.tAB("ActionMenu.setRunning", "0");
        running = enabled;

        if (sSettings.getStoredValue("tracing_started") != enabled) {
            sSettings.setKV("tracing_started", enabled);
        }
        AlogMarker.tAE("ActionMenu.setRunning", "0");
    }

    public int getViewID() {
        AlogMarker.tAB("ActionMenu.getViewID", "0");
        AlogMarker.tAE("ActionMenu.getViewID", "0");
        return R.layout.action_menu;
    }

    public boolean recheckTracingState() {
        boolean enabled = false;

        if ((sSettings.getStoredValue("modemTraces_enabled" + AMTLApplication.getCurrentModemName())
                || sSettings.getStoredValue("logcatTraces_enabled"))) {
            enabled = true;
        }

        if (sSettings.getStoredValue("tracing_started") != enabled) {
            sSettings.setKV("tracing_started", enabled);
            setRunning(enabled);
        }
        return enabled;
    }

    public void attachReferences(View view) {
        AlogMarker.tAB("ActionMenu.attachReferences", "0");
        this.buttonStart = (ImageButton) view.findViewById(R.id.buttonStart);
        this.buttonStop = (ImageButton) view.findViewById(R.id.buttonStop);
        this.buttonSave = (ImageButton) view.findViewById(R.id.buttonSave);
        this.buttonClear = (ImageButton) view.findViewById(R.id.buttonClear);
        this.view = view;
        setRunning(sSettings.getStoredValue("tracing_started"));
        AlogMarker.tAE("ActionMenu.attachReferences", "0");
    }

    public void attachListeners() {
        AlogMarker.tAB("ActionMenu.attachListeners", "0");
        if (this.buttonStart != null)
            this.buttonStart.setOnClickListener(this);

        if (this.buttonStop != null)
            this.buttonStop.setOnClickListener(this);

        if (this.buttonSave != null)
            this.buttonSave.setOnClickListener(this);

        if (this.buttonClear != null)
            this.buttonClear.setOnClickListener(this);
        AlogMarker.tAE("ActionMenu.attachListeners", "0");
    }

    public void onClick(View v) {
        AlogMarker.tAB("ActionMenu.onClick", "0");
        String text = null;

        switch (v.getId()) {
            case R.id.buttonStart:
                if (isRunning()) {
                    text = "Updating tracing";
                    restart();
                } else {
                    text = "Start tracing";
                    start();
                }
                break;
            case R.id.buttonStop:
                text = "Stop tracing";
                stop();
                break;
            case R.id.buttonSave:
                if (toast != null) {
                    toast.cancel();
                }
                toast = Toast.makeText(v.getContext(), "Saving ...", Toast.LENGTH_SHORT);
                String path = save();
                return;
            case R.id.buttonClear:
                cleanTemp();
                break;
            default:
                AlogMarker.tAE("ActionMenu.onClick", "0");
                return;
        }

        if (toast != null) {
            toast.cancel();
        }

        if (text != null) {
            toast = Toast.makeText(v.getContext(), text, Toast.LENGTH_SHORT);
            toast.show();
        }
        AlogMarker.tAE("ActionMenu.onClick", "0");
    }

    public void updateConfiguration() { }

    /******************* Configurations *****************/
    public static void updateConfig(String name, GeneralTracing tracer) {
        AlogMarker.tAB("ActionMenu.addConfig", "0");
        if (tracer != null) {
            tracers.registerTracer(name, tracer);
        }
        AlogMarker.tAE("ActionMenu.addConfig", "0");
    }

    public static boolean hasTracers() {
        return !tracers.getTracers().isEmpty();
    }

    public static ModemConf getModemConfiguration() {
        AlogMarker.tAB("ActionMenu.getModemConfiguration", "0");
        AlogMarker.tAE("ActionMenu.getModemConfiguration", "0");
        return AMTLTabLayout.getModemConfiguration();
    }

    public static boolean getAtProxyStatus() {
        AlogMarker.tAB("ActionMenu.getAtProxyStatus", "0");
        AlogMarker.tAE("ActionMenu.getAtProxyStatus", "0");
        return AMTLTabLayout.getAtProxyStatus();
    }

    private static String getRelativeStorePath() {
        AlogMarker.tAB("ActionMenu.getRelativeStorePath", "0");
        AlogMarker.tAE("ActionMenu.getRelativeStorePath", "0");
        return sSettings.getRelativeStorePath();
    }

    public String getLastStatus() {
        AlogMarker.tAB("ActionMenu.getLastStatus", "0");
        AlogMarker.tAE("ActionMenu.getLastStatus", "0");
        return "";
    }

    public String getTracerName() {
        AlogMarker.tAB("ActionMenu.getTracerName", "0");
        AlogMarker.tAE("ActionMenu.getTracerName", "0");
        return "Action Menu";
    }
}
