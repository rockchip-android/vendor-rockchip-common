/* Android AMTL
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
 * Author: Erwan Bracq <erwan.bracq@intel.com>
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 */

package com.intel.amtl.helper;

import android.os.FileObserver;
import android.util.Log;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.StoredSettings;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;

public class LogManager extends FileObserver {

    private final String TAG = "AMTL";
    private final String MODULE = "LogManager";
    private final String APTRIGGER = "/aplogs/aplog_trigger";
    private final String NBAPFILE = "10";
    private String tag = null;
    private String aplogPath = null;
    private String bplogPath = null;
    private String backupPath = null;
    private String snapPath = null;

    public LogManager(String bckpath, String appath, String bppath) {
        super(appath, FileObserver.ALL_EVENTS);
        AlogMarker.tAB("LogManager.LogManager", "0");
        aplogPath = appath;
        bplogPath = bppath;
        backupPath = bckpath;
        // check backup folder
        if (!this.checkBackupFolder()) {
            AlogMarker.tAE("LogManager.LogManager", "0");
            return;
        }
        AlogMarker.tAE("LogManager.LogManager", "0");
        // right now, only aplog path needs to be monitored to get the created aplog from crash
        // logger.
    }

    public void setTag(String tag) {
        AlogMarker.tAB("LogManager.setTag", "0");
        this.tag = tag;
        AlogMarker.tAE("LogManager.setTag", "0");
    }

    private boolean checkBackupFolder() {
        AlogMarker.tAB("LogManager.checkBackupFolder", "0");
        File file = new File(this.backupPath);
        AlogMarker.tAE("LogManager.checkBackupFolder", "0");
        return file.exists();
    }

    private boolean requestAplog() {
        AlogMarker.tAB("LogManager.requestAplog", "0");
        boolean ret = false;
        BufferedWriter out = null;
        try {
            StoredSettings privatePrefs = new StoredSettings(AMTLApplication.getContext());
            out = new BufferedWriter(new FileWriter(privatePrefs.getRelativeStorePath()
                    + APTRIGGER));
            out.write("APLOG=" + NBAPFILE);
            out.newLine();
            ret = true;
            AlogMarker.tAE("LogManager.requestAplog", "0");
        } catch (IOException e) {
            Log.e(TAG, MODULE + ": Error while writing in file: " + e);
        } finally {
            if (out != null) {
                try {
                    out.close();
                } catch (IOException ex) {
                    Log.e(TAG, MODULE + ": Error during close " + ex);
                }
            }
            AlogMarker.tAE("LogManager.requestAplog", "0");
            return ret;
        }
    }

    private void copyFile(String src, String dst)  {
        AlogMarker.tAB("LogManager.copyFile", "0");
        InputStream in = null;
        OutputStream out = null;
        try {

            Log.d(TAG, MODULE + ": copyFile src: " + src + " to dst: " + dst);

            in = new FileInputStream(new File(src));
            out = new FileOutputStream(new File(dst));

            byte[] buf = new byte[1024];
            int len;

            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
        } catch (IOException e) {
            Log.e(TAG, MODULE + ": COPY issue on object: " + src);
            e.printStackTrace();
        } finally {
            try {
                if (out != null) {
                    out.close();
                }
            } catch (IOException ex) {
                Log.e(TAG, MODULE + ": Error during close " + ex);
            } finally {
                try {
                    if (in != null) {
                        in.close();
                    }
                } catch (IOException ex) {
                    Log.e(TAG, MODULE + ": Error during close " + ex);
                }
            }
        }
        AlogMarker.tAE("LogManager.copyFile", "0");
    }

    private void doLogCopy(final String regex, String searchPath) {
        AlogMarker.tAB("LogManager.doLogCopy", "0");
        FilenameFilter filter = new FilenameFilter() {
            public boolean accept(File directory, String fileName) {
                AlogMarker.tAE("LogManager.doLogCopy", "0");
                return fileName.startsWith(regex);
            }
        };
        File src = new File(searchPath);
        File[] files = src.listFiles(filter);
        if (files != null) {
            for (File cpyfile : files) {
                this.copyFile(searchPath + cpyfile.getName(), this.snapPath + cpyfile.getName());
            }
        }
        AlogMarker.tAE("LogManager.doLogCopy", "0");
    }

    public synchronized boolean makeBackup(String path, boolean requestApplogs) {
        AlogMarker.tAB("LogManager.makeBackup", "0");
        if (path == null || path.length() == 0) {
            AlogMarker.tAE("LogManager.makeBackup", "0");
            return false;
        }

        this.snapPath = path;
        File file = new File(snapPath);
        if ((!file.exists()) && (!file.mkdirs())) {
            AlogMarker.tAE("LogManager.makeBackup", "0");
            return false;
        }

        // Do bplog copy
        this.doLogCopy("bplog", this.bplogPath);

        if (!requestApplogs) {
            AlogMarker.tAE("LogManager.makeBackup", "0");
            return true;
        }

        // Activate the watcher, this allow us to get the created aplog folder by crashlogger.
        this.startWatching();

        // Request aplog snapshot to crashlogger.
        if (!this.requestAplog()) {
            AlogMarker.tAE("LogManager.makeBackup", "0");
            return false;
        }

        AlogMarker.tAE("LogManager.makeBackup", "0");
        return true;
    }

    public synchronized boolean makeBackup() {
        AlogMarker.tAB("LogManager.makeBackup", "0");
        // Create tagged snapshot folder
        SimpleDateFormat df = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String fdf = df.format(new Date());

        AlogMarker.tAE("LogManager.makeBackup", "0");
        return (makeBackup(backupPath + this.tag + "_" + fdf + "/", true));
    }

    @Override
    public synchronized void onEvent(int event, String path) {
        AlogMarker.tAB("LogManager.onEvent", "0");
        if (path == null) {
            AlogMarker.tAE("LogManager.onEvent", "0");
            return;
        }
        //a new file or subdirectory was created under the monitored directory
        if ((FileObserver.CREATE & event) != 0) {
            String lastEventPath = aplogPath + path + "/";
            Log.d(TAG, MODULE + ": CREATED event on object: " + lastEventPath);
            this.stopWatching();

            try {
                Thread.currentThread();
                Thread.sleep(15000);
            } catch (InterruptedException ie) {
                Log.e(TAG, MODULE + ": Sleep interrupted, Aplog backup may be trucated.");
            }
            this.doLogCopy("aplog", lastEventPath);
        }
        AlogMarker.tAE("LogManager.onEvent", "0");
    }
}
