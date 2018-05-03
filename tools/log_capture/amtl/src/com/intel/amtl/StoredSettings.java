/* AMTL
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

package com.intel.amtl;

import com.intel.amtl.helper.FileOperations;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.preference.PreferenceManager;
import android.util.Log;

import com.intel.amtl.R;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

public class StoredSettings {
    protected SharedPreferences appPrivatePrefs;
    private final SharedPreferences appSharedPrefs;
    protected Editor privatePrefsEditor;
    private final Editor sharedPrefsEditor;
    private final Context mCtx;
    private static final String TAG = "AMTL";
    private static final String MODULE = "StoredSettings";
    private static final String LOGCAT_PATH = "/system/bin/logcat";
    private static final String LOGCATEXT_PATH = "/system/bin/logcatext";
    private static final String LOGCATEXT_VENDOR_PATH = "/system/vendor/bin/logcatext";
    private static List<String> availableBuffers = new ArrayList<String>();

    public StoredSettings(Context context) {
        this.mCtx = context;
        this.appPrivatePrefs = context.getSharedPreferences("amtlPrivatePreferences",
                Context.MODE_PRIVATE);
        this.privatePrefsEditor = appPrivatePrefs.edit();
        this.appSharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);
        this.sharedPrefsEditor = appSharedPrefs.edit();
    }

    public int getLogcatFileCount() {
        String logsCount = "5";

        if (null != appSharedPrefs) {
            logsCount = appSharedPrefs.getString(
                    mCtx.getString(R.string.settings_logcat_file_count_key),
                    mCtx.getString(R.string.settings_logcat_file_count_default));
        }

        try {
            return Integer.parseInt(logsCount);
        } catch (NumberFormatException e) {
            return 5;
        }
    }

    public int getLogcatTraceSize() {
        String logsSize = "16";

        if (null != appSharedPrefs) {
            logsSize = appSharedPrefs.getString(
                    mCtx.getString(R.string.settings_logcat_size_key),
                    mCtx.getString(R.string.settings_logcat_size_default));
        }

        try {
            return Integer.parseInt(logsSize);
        } catch (NumberFormatException e) {
            return 16;
        }
    }

    public String getRelativeStorePath() {
        return appSharedPrefs.getString(
                mCtx.getString(R.string.settings_save_path_key),
                mCtx.getString(R.string.settings_save_path_default));
    }

    public String getModemProfile() {
        String mdmProfile = getProfileName(appSharedPrefs.getString(
                mCtx.getString(R.string.settings_modem_profile_key),
                mCtx.getString(R.string.settings_modem_profile_default)));
        return mdmProfile;
    }

    private String getProfileName(String index) {
        String profName = "default";
        int id = Integer.parseInt(index);
        String[] modemProfile = AMTLApplication.getContext().getResources()
                .getStringArray(R.array.modemProfileText);

        if (null != modemProfile) {
            if (id != -1) {
                profName = modemProfile[id];
            }
        }
        return profName;
    }

    public String getBPLoggingPath() {
        return appSharedPrefs.getString(
                mCtx.getString(R.string.settings_bp_path_key),
                mCtx.getString(R.string.settings_bp_path_default));
    }

    public String getBPFileCount() {
        return appSharedPrefs.getString(
                mCtx.getString(R.string.settings_bp_file_count_key),
                mCtx.getString(R.string.settings_bp_file_count_default));
    }

    public String getBPTraceSize() {
        return appSharedPrefs.getString(
                mCtx.getString(R.string.settings_bp_size_key),
                mCtx.getString(R.string.settings_bp_size_default));
    }

    public String getAtProxyMode() {
        return appSharedPrefs.getString(
                mCtx.getString(R.string.settings_at_proxy_mode_key), "");
    }

    public void setAPLoggingPath(String path) {
        sharedPrefsEditor.putString(mCtx.getString(R.string.settings_save_path_key), path);
        sharedPrefsEditor.commit();
    }

    public void setBPLoggingPath(String path) {
        sharedPrefsEditor.putString(mCtx.getString(R.string.settings_bp_path_key), path);
        sharedPrefsEditor.commit();
    }

    public void setBPFileCount(String count) {
        sharedPrefsEditor.putString(mCtx.getString(R.string.settings_bp_file_count_key), count);
        sharedPrefsEditor.commit();
    }

    public void setBPTraceSize(String size) {
        sharedPrefsEditor.putString(mCtx.getString(R.string.settings_bp_size_key), size);
        sharedPrefsEditor.commit();
    }

    public void setAtProxyMode(String mode) {
        sharedPrefsEditor.putString(mCtx.getString(R.string.settings_at_proxy_mode_key), mode);
        sharedPrefsEditor.commit();
    }

    private void refreshActiveLogcatBuffers(List<String> buffers) {
        Iterator<String> it = buffers.iterator();
        while (it.hasNext()) {
            String buffer = it.next();
            boolean bufferPresent = isBufferAvailable(buffer);

            if (appPrivatePrefs != null
                    && appPrivatePrefs.getBoolean(buffer, false) != bufferPresent) {
                privatePrefsEditor.putBoolean(buffer, bufferPresent);
            }
        }

        privatePrefsEditor.commit();
    }

    public static boolean isLogcatExtAvailable() {
        return FileOperations.pathExists(LOGCATEXT_PATH)
                || FileOperations.pathExists(LOGCATEXT_VENDOR_PATH);
    }

    public static String getLogcatPath() {
        String command = null;
        if (FileOperations.pathExists(LOGCATEXT_VENDOR_PATH)) {
            command = LOGCATEXT_VENDOR_PATH;
        } else if (FileOperations.pathExists(LOGCATEXT_PATH)) {
            command = LOGCATEXT_PATH;
        } else if (FileOperations.pathExists(LOGCAT_PATH)) {
            command = LOGCAT_PATH;
        }
        return command;
    }

    public List<String> getActiveLogcatBuffers(boolean refresh, List<String> buffers) {

        if (refresh) {
            refreshActiveLogcatBuffers(buffers);
        }

        Iterator<String> it = buffers.iterator();
        while (it.hasNext()) {
            String buffer = it.next();
            if (appPrivatePrefs != null && !appPrivatePrefs.getBoolean(buffer, false)) {
                it.remove();
            }
        }

        return buffers;
    }

    public List<String> getActiveLogcatBuffers(boolean refresh) {
        String sBuffers[] = new String[] { "kernel", "main", "radio", "crash", "events", "system" };
        List<String> buffers = new ArrayList<>(Arrays.asList(sBuffers));

        return getActiveLogcatBuffers(refresh, buffers);
    }

    public void setKV(String key, boolean value) {
        if (null != privatePrefsEditor) {
            privatePrefsEditor.putBoolean(key, value);
            privatePrefsEditor.commit();
        }
    }

    public boolean getStoredValue(String key) {
        return getStoredValue(key, false);
    }

    public boolean getStoredValue(String key, boolean defaultValue) {
        if (null != appPrivatePrefs) {
            return appPrivatePrefs.getBoolean(key, defaultValue);
        }
        return defaultValue;
    }
    public void setKV(String key, Integer value) {
        if (null != privatePrefsEditor) {
            privatePrefsEditor.putInt(key, value);
            privatePrefsEditor.commit();
        }
    }

    public int getStoredIntValue(String key) {
        return getStoredIntValue(key, -1);
    }

    public int getStoredIntValue(String key, int defaultValue) {
        if (null != appPrivatePrefs) {
            return appPrivatePrefs.getInt(key, defaultValue);
        }
        return defaultValue;
    }

    public static Boolean isBufferAvailable(String name) {
        // logcatext does not display the kernel buffer on logcatext -b all -g
        if (isLogcatExtAvailable() && name.equals("kernel")) {
                return true;
        }

        if (availableBuffers.size() == 0) {
            String command = StoredSettings.getLogcatPath();
            if (command == null) {
                return false;
            }

            try {
                Process proc = Runtime.getRuntime().exec(command + " -b all -g");

                BufferedReader input = new BufferedReader(
                        new InputStreamReader(proc.getInputStream()));

                String s = null;
                while ((s = input.readLine()) != null) {
                    String[] token = s.split(":");
                    availableBuffers.add(token[0]);
                }
                input.close();
            } catch (IOException e) {
                Log.e(TAG, MODULE + ": Could not determine logcat available buffers");
            }
        }

        return availableBuffers.contains(name);
    }
}
