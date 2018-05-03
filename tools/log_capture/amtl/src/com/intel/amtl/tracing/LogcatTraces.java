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
import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.helper.FileOperations;
import com.intel.amtl.service.LogCaptureService;
import com.intel.amtl.StoredSettings;
import com.intel.amtl.R;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

public class LogcatTraces implements GeneralTracing, OnClickListener, OnCheckedChangeListener {
    private static Process sLogcatProc = null;
    private static final String TEMP = "logcat_amtl";


    private static final String TAG = "AMTL";
    private static final String MODULE = "LogcatTraces";
    private static StoredSettings sSettings = null;
    private Button mSelectAllButton = null;
    private static TextView mStatusText = null;

    static Buffer[] mBuffers = {
        new Buffer(R.id.logcatKernel, "kernel"),
        new Buffer(R.id.logcatMain, "main"),
        new Buffer(R.id.logcatRadio, "radio"),
        new Buffer(R.id.logcatCrash, "crash"),
        new Buffer(R.id.logcatEvent, "events"),
        new Buffer(R.id.logcatSystem, "system")
    };

    static List<Buffer> mBuffersList = Arrays.asList(mBuffers);

    static LogcatTraces instance;
    public static LogcatTraces getInstance() {
        return instance;
    }

    private String mLastStatus = "";


    private static boolean isLogCaptureServiceRunning(Context context) {
        ActivityManager manager =
                (ActivityManager) context.getSystemService(context.ACTIVITY_SERVICE);
        for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (LogCaptureService.class.getName().equals(
                    service.service.getClassName())) {
                return true;
            }
        }
        return false;
    }

    private static class Buffer {
        private Switch mSwitch = null;
        private boolean mActive = true;
        private String mName = "";
        private int mId = 0;

        Buffer(int id, String name) {
            mName = name;
            mId = id;

            mActive = StoredSettings.isBufferAvailable(name);
        }

        void attachView(View view) {
            mSwitch = (Switch) view.findViewById(mId);
        }

        void attachReference() {
            if (mSwitch != null) {
                mSwitch.setOnCheckedChangeListener(LogcatTraces.getInstance());
                if (!mActive) {
                    mSwitch.setVisibility(View.GONE);
                }
                mSwitch.setChecked(LogcatTraces.sSettings.getStoredValue(mName));
            }
        }

        String getName() {
            return mName;
        }

        boolean isActive() {
            return mActive;
        }

        boolean isChecked() {
            return (mSwitch == null) ? false
                    : ((mSwitch.getVisibility() == View.VISIBLE)
                    ? mSwitch.isChecked() : false);
        }

        void restoreSelection() {
            if (mSwitch != null) {
                mSwitch.setChecked(LogcatTraces.sSettings.getStoredValue(mName));
            }
        }

        void setCheckedState(boolean state, boolean update) {
            if (mSwitch != null) {
                mSwitch.setChecked(state);
            }
            if (update) {
                LogcatTraces.sSettings.setKV(mName, state);
            }
        }

        void setCheckedState(boolean state) {
            setCheckedState(state, true);
        }

        void updateMemorizedState() {
            if (mSwitch != null
                    && LogcatTraces.sSettings.getStoredValue(mName) != mSwitch.isChecked()) {
                LogcatTraces.sSettings.setKV(mName, isChecked());
            }
        }

        boolean isMemorizedStateChanged() {
            if (mSwitch != null
                    && LogcatTraces.sSettings.getStoredValue(mName) != mSwitch.isChecked()) {
                return true;
            }
            return false;
        }
    }

    private static String getLogcatCommand(List<String> buffers, String outputFile,
            int logsSize, int logsRotate, String param) {
        String command = null;
        if (buffers == null || outputFile == null
                || logsSize == 0 || logsRotate == 0 || param == null) {
            return command;
        }

        command = StoredSettings.getLogcatPath();
        if (command == null) {
            return command;
        }

        for (String buffer : buffers) {
            command += " -b " + buffer;
        }

        command += " -r " + logsSize;
        command += " -f " + outputFile;
        command += " -n " + logsRotate;
        command += " " + param;

        return command;
    }

    private static boolean stopLogcatProcess() {
        Intent aIntent = new Intent(AMTLApplication.getContext(), LogCaptureService.class);
        AMTLApplication.getContext().stopService(aIntent);
        return true;
    }

    private static boolean relaunchLogcatProcess(List<String> buffers, String outputFile,
            int logsSize, int logsRotate, String param) {
        String command = getLogcatCommand(buffers, outputFile, logsSize, logsRotate,  param);

        if (command == null) {
            return false;
        }

        if (AMTLApplication.getContext() == null) return false;

        if (isLogCaptureServiceRunning(AMTLApplication.getContext())) {
            stopLogcatProcess();
        }

        Intent aIntent = new Intent(AMTLApplication.getContext(), LogCaptureService.class);
        aIntent.putExtra(LogCaptureService.EXTRA_PARAMETERS, command);
        AMTLApplication.getContext().startService(aIntent);

        return true;
    }


    public static boolean relaunchLogcatProcess(Context context) {
        StoredSettings settings = new StoredSettings(context);

        if (!settings.getStoredValue("logcatTraces_enabled")) {
            return false;
        }

        List<String> activeBuffers = settings.getActiveLogcatBuffers(false);
        if (activeBuffers.isEmpty()) {
            return false;
        }

        int logsSize = settings.getLogcatTraceSize();
        int logsCount = settings.getLogcatFileCount();

        String command = getLogcatCommand(activeBuffers, settings.getRelativeStorePath() + "/"
                + TEMP, logsSize, logsCount,  "-v threadtime");

        if (command == null) {
            return false;
        }

        if (AMTLApplication.getContext() == null) return false;

        Intent aIntent = new Intent(AMTLApplication.getContext(), LogCaptureService.class);
        if (isLogCaptureServiceRunning(context)) {
            context.stopService(aIntent);
        }

        aIntent.putExtra(LogCaptureService.EXTRA_PARAMETERS, command);
        context.startService(aIntent);

        return true;
    }

    public LogcatTraces() {
        if (sSettings == null) {
            sSettings = new StoredSettings(AMTLApplication.getContext());
        }
        instance = this;
    }

    public String getLastStatus() {
        return mLastStatus;
    }

    public boolean start() {
        boolean active = false;
        int logsCount;
        int logsSize;

        logsCount = sSettings.getLogcatFileCount();
        logsSize = sSettings.getLogcatTraceSize();

        if (isRunning()) {
            stopLogcatProcess();
        }

        List<String> activeBuffers = new ArrayList<String>();
        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            if (buffer.isActive() && buffer.isChecked()) {
                active = true;
                activeBuffers.add(buffer.getName());
            }
        }

        if (!active) {
            mLastStatus = "No logcat traces activated.";
            return false;
        }

        if (!relaunchLogcatProcess(activeBuffers, sSettings.getRelativeStorePath() + "/" + TEMP,
                logsSize, logsCount, "-v threadtime")) {
            mLastStatus = "Logcat traces process could not be initialized";
            return false;
        }

        setRunning(true);

        mLastStatus = "Logcat traces initialized ok";
        return true;
    }

    public void stop() {
        stopLogcatProcess();
        setRunning(false);

        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            buffer.setCheckedState(false);
        }
    }

    private void resetPreferences() {
        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            buffer.setCheckedState(false);
        }
    }

    public void restoreSelections() {
        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            buffer.restoreSelection();
        }
    }


    public void setState(boolean state) {
        // nothing to do
    }

    public boolean restart() {
        return start();
    }

    public boolean isUpdated(View view) {
        attachReferences(view);

        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            if (buffer.isActive() && buffer.isMemorizedStateChanged()) {
                return true;
            }
        }
        return false;
    }

    public String toString() {
        String str = "";

        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            if (buffer.isActive()) {
                str += buffer.getName() + ": " + buffer.isChecked();
            }
        }

        return str;
    }


    public void cleanTemp() {
        boolean isRunning = isRunning();
        if (isRunning) {
            stop();
        }

        FileOperations.removeFiles(sSettings.getRelativeStorePath() + "/", TEMP);

        if (isRunning) {
            start();
        }
    }

    public void saveTemp(String path) {
        try {
            FileOperations.copyFiles(sSettings.getRelativeStorePath() + "/", path, TEMP);
        } catch (IOException e) {
            Log.e(TAG, MODULE + ": Could not save logcat log");
        }
    }

    public boolean isRunning() {
        return sSettings.getStoredValue("logcatTraces_enabled");
    }

    public int getViewID() {
        return R.layout.trace_logcat;
    }

    public void attachReferences(View view) {
        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            buffer.attachView(view);
        }

        mSelectAllButton = (Button) view.findViewById(R.id.selectAllLogcatTraces);
        mStatusText = (TextView) view.findViewById(R.id.logcatLoggingStatusValue);
    }

    public void attachListeners() {
        boolean logcatTracingStarted = sSettings.getStoredValue("logcatTraces_enabled");
        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            buffer.attachReference();
        }

        if (mSelectAllButton != null) {
            mSelectAllButton.setOnClickListener(this);
        }

        updateStatusMessage();
    }

    private void updateStatusMessage() {
        if (mStatusText != null) {
            mStatusText.setText(
                    sSettings.getStoredValue("logcatTraces_enabled") ? " running" : " stopped");
        }
    }

    private void setRunning(boolean enabled) {

        sSettings.setKV("logcatTraces_enabled", enabled);
        updateStatusMessage();

        if (!enabled) {
            return;
        }

        Iterator<Buffer> it = mBuffersList.iterator();
        while (it.hasNext()) {
            Buffer buffer = it.next();
            buffer.updateMemorizedState();
        }
    }

    public void updateConfiguration() { }

    public void onCheckedChanged (CompoundButton v, boolean isChecked) {
        ((OnLogcatTraceModeApplied) AMTLApplication.getActivity())
                .onLogcatTraceConfApplied(this);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.selectAllLogcatTraces:
                Iterator<Buffer> it = mBuffersList.iterator();
                while (it.hasNext()) {
                    Buffer buffer = it.next();
                    buffer.setCheckedState(true, false);
                }
                break;
        }
    }

    public interface OnLogcatTraceModeApplied {
        public void onLogcatTraceConfApplied(GeneralTracing lt);
    }

    private static OnLogcatTraceModeApplied nullCb = new OnLogcatTraceModeApplied() {
        public void onLogcatTraceConfApplied(GeneralTracing lt) { }
    };

    public String getTracerName() {
        return "Logcat Traces";
    }
}
