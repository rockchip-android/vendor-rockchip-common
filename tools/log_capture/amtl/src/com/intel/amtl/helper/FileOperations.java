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

package com.intel.amtl.helper;

import android.os.Environment;
import android.util.Log;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.log.AlogMarker;

import java.io.File;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class FileOperations {

    private static final String TAG = "AMTL";
    private static final String MODULE = "FileOperations";
    public static String BP_LOG_FILE_NAME_MATCH = "bplog";

    public static boolean pathExists(String path) {

        AlogMarker.tAB("FileOperation.pathExists", "0");
        File file = new File(path);
        if (null == file || !file.exists()) {
            AlogMarker.tAE("FileOperation.pathExists", "0");
            return false;
        }

        AlogMarker.tAE("FileOperation.pathExists", "0");
        return true;
    }

    public static boolean removeFile(String path) {
        AlogMarker.tAB("FileOperation.removeFile", "0");
        Log.d(TAG, MODULE + ": starting removing " + path);
        File file = new File(path);
        if (null == file) {
            AlogMarker.tAE("FileOperation.removeFile", "0");
            return false;
        }

        if (!file.exists()) {
            AlogMarker.tAE("FileOperation.removeFile", "0");
            return true;
        }

        if (!file.delete()) {
            AlogMarker.tAE("FileOperation.removeFile", "0");
            return false;
        }
        Log.d(TAG, MODULE + ": removing " + path + " succedeed");
        AlogMarker.tAE("FileOperation.removeFile", "0");
        return true;
    }

    public static void copy(String src, String dst) throws IOException {
        AlogMarker.tAB("FileOperation.copy", "0");
        Log.d(TAG, MODULE + ": copying " + src + " to " + dst);
        InputStream in = new FileInputStream(src);
        OutputStream out = null;
        byte[] buf = new byte[1024];
        int len;

          try {
            out = new FileOutputStream(dst);
            try {
                while ((len = in.read(buf)) > 0) {
                    out.write(buf, 0, len);
                }
                out.flush();

            } finally {
                out.close();
            }
        } finally {
            in.close();
        }
        AlogMarker.tAE("FileOperation.copy", "0");
    }

    public static void copyFiles(String source, String destination, final String pattern)
            throws IOException {

        AlogMarker.tAB("FileOperation.copyFiles", "0");
        FilenameFilter filter = new FilenameFilter() {
            public boolean accept(File directory, String fileName) {
                AlogMarker.tAE("FileOperation.copyFiles", "0");
                return fileName.startsWith(pattern);
            }
        };
        File src = new File(source);
        File[] files = src.listFiles(filter);
        if (files != null) {
            for (File copy : files) {
                copy(source + copy.getName(), destination + copy.getName());
            }
        }
        AlogMarker.tAE("FileOperation.copyFiles", "0");
    }

    public static void removeFiles(String directory, final String pattern) {

        AlogMarker.tAB("FileOperation.removeFiles", "0");
        FilenameFilter filter = new FilenameFilter() {
            public boolean accept(File directory, String fileName) {
                AlogMarker.tAE("FileOperation.removeFiles", "0");
                return fileName.startsWith(pattern);
            }
        };
        File src = new File(directory);
        File[] files = src.listFiles(filter);
        if (files != null) {
            for (File file : files) {
                removeFile(directory + file.getName());
            }
        }
        AlogMarker.tAE("FileOperation.removeFiles", "0");
    }

    public static String getTimeStampedPath(String path, String tag) {
        AlogMarker.tAB("FileOperation.getTimeStampedPath", "0");
        DateFormat dateFormat = new SimpleDateFormat("yyyy_MM_dd_HHmmss");
        Date date = new Date();

        if (path == null || path.isEmpty()) {
            path = "";
        }

        if (tag == null || tag.isEmpty()) {
            tag = "";
        }

        path += "/" + tag + dateFormat.format(date);
        AlogMarker.tAE("FileOperation.getTimeStampedPath", "0");
        return path + "/";
    }

    public static boolean createPath(String path) throws IOException {
        AlogMarker.tAB("FileOperation.createPath", "0");
        File file = new File(path);

        if ((!file.exists()) && (!file.mkdirs())) {
            AlogMarker.tAE("FileOperation.createPath", "0");
            return false;
        }

        AlogMarker.tAE("FileOperation.createPath", "0");
        return true;
    }

    public static String createCrashFolder(String path) throws IOException {
        AlogMarker.tAB("FileOperation.createCrashFolder", "0");
        File file = new File(path);

        if ((!file.exists()) && (!file.mkdirs())) {
            AlogMarker.tAE("FileOperation.createCrashFolder", "0");
            return "";
        }

        DateFormat dateFormat = new SimpleDateFormat("yyyy_MM_dd_HHmmss");
        Date date = new Date();
        path += "/" + dateFormat.format(date);

        file = new File(path);

        if ((!file.exists()) && (!file.mkdirs())) {
            AlogMarker.tAE("FileOperation.createCrashFolder", "0");
            return "";
        }

        AlogMarker.tAE("FileOperation.createCrashFolder", "0");
        return path + "/";
    }

    public static boolean isSdCardAvailable() {
        AlogMarker.tAB("FileOperation.isSdCardAvailable", "0");
        AlogMarker.tAE("FileOperation.isSdCardAvailable", "0");
        return Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED);
    }

    public static String getSDStoragePath() {
        AlogMarker.tAB("FileOperation.getSDstoragePath", "0");
        AlogMarker.tAE("FileOperation.getSDstoragePath", "0");
        return Environment.getExternalStorageDirectory().getAbsolutePath();
    }
}
